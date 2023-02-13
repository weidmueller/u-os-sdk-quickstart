#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

const char kUioDev[] = "/dev/uio1"; /**<This is the path and name of our UIO device.*/
const char kUioMap[] = "/sys/class/uio/uio1/maps/map0/size"; /**<This is the path to our UIO device's shared memory mapping.*/

/**
 * @file main_triple.c
 * @author Dipl.-Inf. Tobias Klug
 * @date January 23rd 2023
 * @brief A code example for UR20 I/O process data access.
 *
 * This file demonstrates access to I/O process data of UR20 modules attached
 * to a UC20-WL2000 PLC. A kernel module handles the backplane bus and exposes
 * a UIO device. The data transfer to the application is via the shared memory
 * region of that UIO device. 
 * 
 * This example code explains the process data exchange. For information on 
 * the parametrization of the UR20 modules, please read the comments and code
 * in tools/unix_sock.py.
 * 
 * The /dev/uio1 device provides the following data in the shared memory:
 * - information on the shared memory region itself in a ShmRegionInfo struct
 * - information on two sets of triple buffers for the I/O process data 
 * exchange in TripleBufInfo structs
 * - input triple buffer set
 * - output triple buffer set
 * 
 * Each triple buffer set consists of three buffers; we conveniently represent
 * them as an array[3] of (uint8_t*). Thus we can select the read-, the write-,
 * and the available buffer via the associated buffer array index.
 * Each buffer consists of a 16 byte header and 1024 bytes of payload. For the
 * detailed layout of the shared memory region, refer to src/triple_buf_shm.h.
 *
 * The coordination with our UIO device is as follows:
 * 
 * To send data to our UIO device..
 * - we write the data into the out_buffer set's write_buf,
 * - then we atomically take the lock and 
 * - exchange the write_buf index with the available_buf index, and 
 * - set the new_data flag. 
 * - Then we release the lock.
 * 
 * Our UIO device will now pass the data to the modules.
 * 
 * To receive data from our UIO device, we check the new_data flag in in_buf;
 * New data is available if it is set. To obtain these data, ..
 * - we take the lock and 
 * - exchange the read_buf index with the available_buf index and 
 * - clear the new_data flag.
 * - Then we release the lock.
 * 
 * The new data are now in the read_buf and we can read it from there at our 
 * leisure.
 */

/**
 * @brief provides status information on a triple buffer set.
 *
 * A triple buffer set consists of:
 * - three indices pointing to the read-, write- and available buffer
 * - a semaphore that lets us lock the triple buffer set for mutually exclusive access
 * - a flag that indicates availability of new data
 */
typedef struct __attribute__((packed)) {
  volatile uint32_t lock; /**< Use this semaphore to atomically lock the buffer and thus synchronize with our UIO device. */
  volatile uint8_t read_buf; /**< This is the read buffer index. Read incoming I/O process data from the read buffer. */
  volatile uint8_t write_buf; /**< This is the write buffer index. Write outgoing I/O process data into the write buffer. */
  volatile uint8_t available_buf; /**< This is the available buffer index. The other side will procss the available buffer, next. */
  volatile uint8_t new_data; /**< This is the new data flag. We and the UIO device signal each other the availability of new data.
                                  1 indicates new data, 0 indicates no new data. */
} TripleBufInfo;

/**
 * @brief This struct type provides basic information on the memory our UIO device shares with us:
 *
 * The shared memory region information consists of:
 * - a magic number. If the magic number reported by the UIO device is not
 *   0xABCDEFAB, then the backplane bus driver is not working or we have 
 *   addressed the wrong UIO device.
 * - information on the input triple buffer set contained in the memory we share with our UIO device. 
 * - information on the output triple buffer set contained in the memory we share with our UIO device.
 *
*/
typedef struct __attribute__((packed)) {
  uint8_t valid[4]; /**< This is the magic number to verify proper interoperation with our UIO device. */
  TripleBufInfo in_buffer; /**< This provides information on the input process data triple buffer set. */
  TripleBufInfo out_buffer; /**< This provides information on the output process data triple buffer set. */
} ShmRegionInfo;

/**
 * @brief This struct holds timing variables for the periodic I/O access in
 * this code example.
 * 
 * After each finished data exchange with the UIO device, we advance next_period
 * to the time for the next data exchange.
 */
typedef struct {
  struct timespec next_period; /**< This is the time for the next I/O data exchange. */
  long period_ns; /**< This is the period time of our data exchange cycle, measured in ns. */
} PeriodInfo;

/**
 * @brief This locks a triple buffer set for mutually exclusive access.
 * 
 * We use __sync_lock_test_and_set() to take a lock on the triple buffer set _buffer_.
 * If the lock is already taken, we usleep(10) and then try again.
 * See https://gcc.gnu.org/onlinedocs/gcc-4.1.0/gcc/Atomic-Builtins.html for a description
 * of __sync_lock_test_and_set().
 * @param[in] buffer The buffer to lock.
 */
inline void LockTripleBuf(TripleBufInfo* buffer) {
  while (__sync_lock_test_and_set(&buffer->lock, 1)) {
    while (buffer->lock) {
      usleep(10);
    }
  }
}

/**
 * @brief This unlocks a triple buffer set we have previously locked for mutually exclusive access.
 * 
 * We use __sync_lock_release() to release a lock on the triple buffer set _buffer_.
 * See https://gcc.gnu.org/onlinedocs/gcc-4.1.0/gcc/Atomic-Builtins.html for a description
 * of __sync_lock_release().
 * @param[in] buffer The buffer to release.
 */
inline void UnlockTripleBuf(TripleBufInfo* buffer) {
  __sync_lock_release(&buffer->lock);
}

/**
 * @brief This function returns the shared memory size reported by a UIO device mapping.
 * 
 * It assumes that _sysfs_path_file_ points to the map size file of a UIO device. It opens
 * the map size file and reads the shared memory size string from it. Then it converts that
 * string to a 64 bit number and returns that number.
  * @param[in] sysfs_path_file The path and name of the UIO device mapping.
  * @return size The size of the shared memory region.
 */
uint64_t GetMapSize(const char* sysfs_path_file) {
  FILE* size_fd;
  uint64_t size = 0;

  size_fd = fopen(sysfs_path_file, "r");

  if (!size_fd) {
    printf("GetMapSizefailed!\n");
  }

  fscanf(size_fd, "0x%08X", &size);
  fclose(size_fd);

  return size;
}

/**
 * @brief This updates _next_period_ in the _info_ time keeping struct by one cycle time.
 * @param[in] info Our time keeping struct.
 */
void IncPeriod(PeriodInfo* info) {
  info->next_period.tv_nsec += info->period_ns;

  while (info->next_period.tv_nsec >= 1000000000) {
    info->next_period.tv_sec++;
    info->next_period.tv_nsec -= 1000000000;
  }
}

/**
 * @brief This function initializes our _info_ time keeping struct.
 * It sets period_ns to _cycle_ns and sets next_period to the current system clock value.
 * @param[in] info Our time keeping struct.
 * @param[in] cycle_ns The cycle time for our I/O access.
 */
void PeriodicTaskInit(PeriodInfo* info, const long cycle_ns) {
  info->period_ns = cycle_ns;
  clock_gettime(CLOCK_MONOTONIC, &(info->next_period));
}

/**
 * @brief Goes to sleep until the end of our current cycle.
 * It calls IncPeriod() to update the _info_ time keeping struct, then it goes
 * to sleep until the system clock is past info->next_period.
 * @param[in] info Our time keeping struct.
 */
void WaitRestOfPeriod(PeriodInfo* info) {
  IncPeriod(info);
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &info->next_period, NULL);
}

/**
 * @brief This app demonstrates cyclic data exchange with UR20 I/O modules.
 * 
 * It maps the shared memory region of a UC20-WL2000 backplane bus UIO
 * device into its address space and then cyclicly exchanges I/O process data
 * with the UR20 modules attached to the PLC.
 */
int main(int argc, char* argv[]) {
/*
Open a file descriptor to our UIO device: /dev/uio1 represents I/O process data
of UR20 modules attached  to the UC20-WL2000. For general information on UIO
devices, read https://www.kernel.org/doc/html/v4.18/driver-api/uio-howto.html.
*/
  int uio_fd = open(kUioDev, O_RDWR);
  if (uio_fd < 1) {
    printf("open failed!\n");
    exit(1);
  }

/*
Obtain the map size of our UIO device. That is, ask the OS for the size of the
shared memory our UIO device /dev/UIO1 provides. This information is available
in /sys/class/uio/uio1/maps/map0/size. We need to know the shm size to map the
shared memory into our address space, next.
*/
  int map_size = GetMapSize(kUioMap);
  if (map_size == 0) {
    printf("Map size is zero!\n");
    exit(1);
  }

  printf("MapSize: %x\n", map_size);

/*
Tell the OS to map the UIO shared memory into our address space.
See https://man7.org/linux/man-pages/man2/mmap.2.html on how mmap() works.
*/
  void* mem_pointer =
      mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, uio_fd, 0);
  if (mem_pointer == NULL) {
    printf("mmap failed!\n");
    exit(1);
  }

  printf("MemPointer: %x\n", mem_pointer); /* This is the pointer to the shared
  memory. */

  /* 
  Next change our scheduling policy and -priority with sched_setscheduler().
  See https://www.tutorialspoint.com/unix_system_calls/sched_setscheduler.htm
  for a description of sched_setscheduler() and what effect the SCHED_FIFO policy
  has. 
  */
  struct sched_param param;
  param.sched_priority = 81;
  int ret = sched_setscheduler(0, SCHED_FIFO, &param);
  if (ret != 0) {
    printf("sched_setscheduler failed!\n");
    exit(1);
  }

/*
We set our CPU affinity to 1: That is, we tell the OS to let us run only on
core 0. This prevents performance loss due to cache invalidation etc.
See https://man7.org/linux/man-pages/man2/sched_setaffinity.2.html for a
description of sched_setaffinity().
*/
  int affinity = 1;
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(affinity, &mask);
  ret = sched_setaffinity(0, sizeof(cpu_set_t), &mask);
  if (ret != 0) {
    printf("sched_setaffinity failed!\n");
    exit(1);
  }

/*
We lock all our memory, that is, we prevent the system from swapping out
our memory pages to a swap medium. This affects both pages we have _now_ 
and pages we obtain in the future, including shared memory.
See https://man7.org/linux/man-pages/man2/mlock.2.html for details.
*/
  ret = mlockall(MCL_CURRENT | MCL_FUTURE);
  if (ret != 0) {
    printf("mlockall failed!\n");
    exit(1);
  }

/*
Initialize our time-keeping: we want a task cycle of 100 * 1000 * 1000 ns,
that is 100 ms.
*/
  static PeriodInfo period_info;
  PeriodicTaskInit(&period_info, 100 * 1000000);

/*
We cast the shared memory pointer to a pointer to ShmRegionInfo. This way,
we interpret the beginning of the shared memory region as usage information
on our UIO device shared memory.
*/
  ShmRegionInfo* shm_region = (ShmRegionInfo*)mem_pointer;

/*
The UC20's uc_slio_service backplane bus driver provides a magic number in the
shared memory information struct. This lets us check the validity of the shared
memory.
*/
  if (shm_region->valid[0] != 0xAB) {
    printf("Shm region is not valid!\n");
    exit(1);
  }
  /* Print the magic number reported by our UIO device. It shall be 32 bit and
  contain 0xAB|0xCD|0xEF|0xAB. */
  uint8_t i;  
  for (i = 0; i<4; i++){
    printf("shm_region->valid[%d] = 0x%x.\n",i,shm_region->valid[i]);
  }

/*The shared memory our UIO device shares with us also contains two sets of
  triple buffers . We need pointers to their start addresses.*/
  void* triple_buf_in[3];
  void* triple_buf_out[3];

/*The buffers are seamlessly positioned one after the other in the shared
  memory. Thus we can derive their addresses in our address space from the
  sizes of ShmRegionInfo and the buffer sizes. For details, please refer to
  src/triple_buf_shm.h.*/
  int buffer_size = 1040;
  triple_buf_in[0] = (uint8_t*)mem_pointer + sizeof(ShmRegionInfo);
  triple_buf_in[1] = (uint8_t*)triple_buf_in[0] + buffer_size;
  triple_buf_in[2] = (uint8_t*)triple_buf_in[1] + buffer_size;
  triple_buf_out[0] = (uint8_t*)triple_buf_in[2] + buffer_size;
  triple_buf_out[1] = (uint8_t*)triple_buf_out[0] + buffer_size;
  triple_buf_out[2] = (uint8_t*)triple_buf_out[1] + buffer_size;

/* Initialize a counter variable to have something we can send to the process
   output data. Hint: Attach a DO16 to the PLC. The channel's LEDs visualize
   the counter while the code executes. */
  uint8_t counter = 0;

/* loop 'forever', i.e. until we receive a SIGINT. */
  for (;;) {

    counter++;

    /* Get a pointer to byte for the output process data's write buffer. */
    uint8_t* pData = (uint8_t*)triple_buf_out[shm_region->out_buffer.write_buf];
    
    /* Each buffer has a 16 byte header and 1024 byte payload. The first two
       byte in the header is the offset to the start of the process data. */
    
    /* Declare the pointer pOffset and assign our data pointer: It also points
      to the beginning of the buffer's header.*/
    uint16_t* pOffset = (uint16_t*)pData;
    
    /* Set the offset between the buffer address and the process data address
       to 16. Thus, we skip the header and adress the process data memory.*/
    *pOffset = 16;

    /* Use a simple counter as "process data". */
    pData[*pOffset] = counter;

    /* We need a lock on the output process data triple buffer set in order to
       safely manipulate the buffer indices and not interfere with the UIO
       device kernel module. */
    LockTripleBuf(&shm_region->out_buffer);
    
    /* We have placed our output process data - the counter - into the write
       buffer. Now we switch the write buffer index against the available 
       buffer index. This lets the UIO device use _our_ write buffer content
       as _its_ new available buffer content.*/
    uint8_t temp = shm_region->out_buffer.write_buf;
    shm_region->out_buffer.write_buf = shm_region->out_buffer.available_buf;
    shm_region->out_buffer.available_buf = temp;
    
    /* Tell the UIO device kernel module, that new data are available in the
       available buffer. The UIO device kernel module sents them to the UR20
       modules in the next backplane bus cycle.*/
    shm_region->out_buffer.new_data = 1;

    /* We must unlock the triple buffer set after we have finished our content
       changes. The UIO device kernel module can then take the lock and
       evaluate the new data.*/
    UnlockTripleBuf(&shm_region->out_buffer);

    /* check if there are new data in the input process data triple buffer.*/
    if (shm_region->in_buffer.new_data != 0) {

      /* We tell the UIO device kernel module that we are right now processing
         the new data by taking the lock on the triple buffer set.*/
      LockTripleBuf(&shm_region->in_buffer);

      /* Switch the available buffer index and the read buffer index.*/
      uint8_t temp = shm_region->in_buffer.read_buf;
      shm_region->in_buffer.read_buf = shm_region->in_buffer.available_buf;
      shm_region->in_buffer.available_buf = temp;
      
      /* clear the new data flag to indicate that we have processed the data.*/
      shm_region->in_buffer.new_data = 0;

      /* We must unlock the triple buffer set after we have finished our
       changes. The UIO device kernel module can then take the lock and
       provide new data, if any are available.*/
      UnlockTripleBuf(&shm_region->in_buffer);
      // printf("switch buffer\n");
      // fflush(stdout);
    }

    /* We need a pointer to the input process data triple buffer in order
       to obtain a pointer to the payload offset. */
    pData = (uint8_t*)triple_buf_in[shm_region->in_buffer.read_buf];
    
    /* The payload offset is at position 0 in the buffer.*/
    pOffset = (uint16_t*)pData;
    // printf("data read: %x\n", pData[*pOffset]);
    // fflush(stdout);

    /* We have finished our business and go to sleep for the rest of our cylce*/
    WaitRestOfPeriod(&period_info);
  }

  /* We un-map the shared memory region from our address space to clean up
     before we terminate. */
  if (munmap(mem_pointer, map_size) != 0) {
    printf("munmap failed!\n");
    exit(1);
  }

  /* Clear the pointer to make sure nobody thinks it might point to something
      useful. */
  mem_pointer = NULL;

  /* Close the file descriptor to the UIO device to let the OS free up 
     ressources it may have reserved for us. */
  if (close(uio_fd) != 0) {
    printf("close failed!\n");
    exit(1);
  }
  
  return 0;
}