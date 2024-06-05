// Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
// SPDX-FileCopyrightText: 2023 
//
// SPDX-License-Identifier: Apache-2.0

/* includes */
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <openssl/opensslv.h>
#include <openssl/crypto.h>
#include <mosquitto.h>

/**
 * \mainpage Introduction
 * 
 * The VSC projects "helloworld_mqtt", "openssl" and "mosquitto" together demonstrate how to use
 * autotools- and cmake-built libraries with the UOS-SDK. Follow these build instructions: @n
 * The helloworld_mqtt code links against the mosquitto- and openssl 3.0 libraries. These are not
 * part of the UOS-SDK installer; @n Build and install them by executing the following VSC tasks
 * (VSC Menu: Terminal -> Run Task):
 * - configure openSSL
 * - make openssl
 * - make install openssl
 * - mosquitto: cmake
 * - mosquitto: make install
 * 
 * Both \e make \e install calls will install the respective libraries in the SDK's
 * target sysroot. The call parameters for configure and cmake pass the needed
 * information. @n
 * Next, execute the following VSC tasks to configure and build helloworld_mqtt:
 * - cmake helloworld_mqtt
 * - make helloworld_mqtt
 * 
 * Peruse the /.vscode/tasks.json files in openssl/, mosquitto/ and helloworld_mqtt/ to see how to
 * run autotools configure, cmake and make via VSC project tasks.
 * 
 * Please find a description of what the helloworld_mqtt example binary does in main.c.
 * 
 * \sa https://github.com/eclipse/mosquitto and https://github.com/openssl/openssl
 */

/**
 * @file main.c
 * @author w010174
 * @date March 15th 2023
 * @brief A code example for MQTTS with UOS.
 *
 * The example initializes itself as MQTTS client, connects to a broker and publishes cyclicly
 * until a SIGINT occurs. Then it disconnects from the broker and shuts down.
 * 
 */

/* global declarations */
bool xConnected = false;
bool xSIGINT = false;

/**
 * @brief mosquitto's "on connect" callback function
 * 
 * Informs the user of a successful (re)connection to the MQTT(S) broker and
 * sets xConnected.
 */
void callback_on_connect(struct mosquitto * mosq, void * obj, int rc){

   printf("callback on connect: rc is 0x%x.\n", rc);

   /* set global flag to tell main() that we've connected to the broker and it can stop waiting with timeout.. */
   xConnected = true;
}

/**
 * @brief mosquitto's "on disconnect" callback function
 * 
 * Informs the user of an intentional or unintentional disconnect from the MQTT(S) broker.
 * Clears xConnected.
 */
void callback_on_disconnect(struct mosquitto * mosq, void * obj, int rc){

   printf( "callback on disconnect: rc is 0x%x.\n", rc);
   {
      /* was this an intentional disconnect? */
      if(rc == 0){
         /* yes -> trigger event "disconnected from broker" */
         printf("intentional disconnect.\n");
        }
      else {
         /* no -> trigger event "connection to broker lost" */
         printf("connection to broker lost.\n");
      }

      /* clear global flag to tell main() that we've disconnected from the broker. 
      Main shall stop its publishing loop and exit. */
      xConnected = false;
   }

}

/**
 * @brief a simple SIGINT signal handler 
 * 
 * Informs the user of an incoming SIGINT and sets xSIGINT.
 */
void signalHandler( int signum ) {

   printf( "Interrupt signal ( 0x%x ) received.\n",signum);

   /* tell the publish loop to exit */
   xSIGINT = true;
}

/**
 * @brief The example's main code.
 * 
 * It queries and prints the openSSL version info of the actual openSSL version found.
 * Then it takes the following initialization steps:
 * - initialize the mosquitto library and create a client instance
 * - register connect- and disconnect callback functions
 * - start the messaging loop
 * - set up the CA file name for MQTTS
 * - asynchronously connect to the broker test.mosquitto.org
 *    - do this in an endless retry loop every 5s until connected or SIGINT occurs
 * - on connect, register our own SIGINT handler
 * 
 * After successful completion of these steps, the code enters a loop and publishes
 * a test message cyclicly every 5s to a hardcoded topic until a SIGINT occurs.
 * On SIGINT, the code takes the following shutdown steps:
 * - disconnect from broker
 * - wait for the disconnect confirmation
 * - stop the messaging loop
 * - destroy client instance
 * - exit
 * 
 * While doing the cyclic publishing, the code also prints out error information if publishing
 * failed.
 * 
 * \sa https://mosquitto.org/api/files/mosquitto-h.html
 */
int main(){
   uint8_t uiWaitSeconds = 0; /* counter for the wait-for-connect-to-broker timeout */
   libmosq_EXPORT struct mosquitto * pMosq; /* our mosquitto library */
   int32_t iMosqResult; /* results of mosquitto API function calls */
   char* currentTopicName = "WI_example_topic"; /* the topic we publish to */
   char* strPayload = "{\"testmessage\" : \"Hello, Wolrd!\"}"; /* the publish message */
   
   printf("This is the hello world MQTT example.\n");

   /* get version information from the actual openSSL library we linked against */
   unsigned int uiOpenSSL_version_major = OPENSSL_version_major();
   unsigned int uiOpenSSL_version_minor = OPENSSL_version_minor();
   unsigned int uiOpenSSL_version_patch = OPENSSL_version_patch();
   const char * ccOpenSSL_version_pre_release = OPENSSL_version_pre_release();

   printf("Service has found OpenSSL version %d.%d.%d.%s\n",
                                       uiOpenSSL_version_major, \
                                       uiOpenSSL_version_minor, \
                                       uiOpenSSL_version_patch, \
                                       ccOpenSSL_version_pre_release );

   const char * ccOpenSSL_version_build_metadata = OPENSSL_version_build_metadata();
   if (strlen(ccOpenSSL_version_build_metadata))
      printf("OpenSSL meta data: %s\n", ccOpenSSL_version_build_metadata);

   /* initialize mosuqitto library*/
   printf("mosquitto_lib_init()..\n");
   mosquitto_lib_init();

   /* create a mosquitto client instance */
   printf("mosquitto_new()..\n");
   pMosq = mosquitto_new("WI_SDK_example_client", true, NULL);

   /* register the connect- and disconnect callback functions with our mosquitto client instance */
   printf("mosquitto: register callbacks..\n");
   mosquitto_connect_callback_set(pMosq, &callback_on_connect);
   mosquitto_disconnect_callback_set(pMosq, &callback_on_disconnect);

    /* mosquitto_loop_start() starts the message processing loop */
   printf("starting MQTT message loop..\n");
   iMosqResult = mosquitto_loop_start(pMosq);

   /* check the result of mosquitto_loop_start(); */
   if(iMosqResult != MOSQ_ERR_SUCCESS){

      /* we could not start the mosquitto MQTT message loop. Report the error and shut down orderly. */
      printf( "%s",  mosquitto_strerror(iMosqResult));
      printf("error starting loop: 0x%x\n", iMosqResult);
      printf("destroy mosquitto client instance.\n");
      mosquitto_destroy(pMosq);

      /* leave this program. */
      exit(-1);
   }

/* set mosquitto's TLS parameters */
char* strCAfilename = "/tmp/mosquitto.org.crt";
        iMosqResult = mosquitto_tls_set(pMosq, strCAfilename, \
                                            NULL, \
                                            NULL, \
                                            NULL,
                                            NULL );

if (iMosqResult != MOSQ_ERR_SUCCESS) {
   printf("mosquitto_tls_set failed, 0x%d.\n", iMosqResult);
   return -1;
};

lCONNECT_AGAIN: /* <-- we jump here if the connection attempt fails because there's no internet connection. */
    /* let mosquitto connect to the broker */
   printf("mosquitto_connect_async()..\n");

   /* uncomment the next line for MQTT without TLS: */
//   iMosqResult= mosquitto_connect_async(pMosq, "test.mosquitto.org", 1883, 10);

   /* uncomment the next line for CA certificate based MQTTS */
   iMosqResult= mosquitto_connect_async(pMosq, "test.mosquitto.org", 8883, 10);

   /* evaluate the result of the connection attempt */
   if((iMosqResult != MOSQ_ERR_SUCCESS) && (iMosqResult != MOSQ_ERR_EAI)){

      printf( "cannot connect to broker. Mosquitto error string is: %s\n" ,mosquitto_strerror(iMosqResult));

      /* force stop of mosquitto network communication loop. */
      printf("mosquitto_loop_stop()..\n");
      mosquitto_loop_stop(pMosq, true);

      /* destroy our mosquitto client instance. */
      printf( "destroy mosquitto client instance.\n");
      mosquitto_destroy(pMosq);

      /* leave this program */
      printf("exit(2)..\n");
      exit(-2);
   }

  if (iMosqResult == MOSQ_ERR_EAI){
      /* tell the user that we could'nt find the broker. Mostly this happens because we have no internet access. */
      printf( "Lookup error occurred. Retry in 5s.\n");
      /* wait five seconds. */
      sleep(5);
      /* try again by jumping to the mosquitto_connect_async() call above. */
      goto lCONNECT_AGAIN;
   }
   else{
      /* mosquitto_connect_async() succeeded. Now wait e.g. 10s for the callback-on-connect's report. */
      printf("mosquitto_connect_async() succeeded. Now wait for the callback-on-connect.\n");
      /* reset the timeout counter */
      uiWaitSeconds = 0;
      /* wait second-wise.. */
      while(uiWaitSeconds < 10){
         /* check if the mosquitto client has established a connection to the MQTT broker */
         if(xConnected) break;
         /* wait one second and count the seconds */
         sleep(1);
         uiWaitSeconds++;
      }

      /* tell the user how long we waited and if we established a connection, or not */
      printf("We waited %ds for the connection to the broker.\n",uiWaitSeconds);
      printf("xConnected is: %d\n",xConnected);

      /* did the mosquitto client connect to the MQTT broker? */
      if(xConnected){
         /* register our own signal handler for SIGINT */
         printf("registering the SIGINT handler..\n");
         signal(SIGINT, signalHandler);

         printf("\nEntering the publish loop.\n");
      } else{

         printf("connection to broker timed out.\n");

         /* force stop of mosquitto network communication loop. */
         printf("mosquitto_loop_stop()..\n");
         mosquitto_loop_stop(pMosq, true);

         /* destroy our mosquitto client instance. */
         printf( "destroy mosquitto client instance.\n");
         mosquitto_destroy(pMosq);

         /* leave this program */
         printf("exit(3)..\n");
         exit(-3);
      }
  
      /* keep sending messages while we are connected to the broker. */
      while(xConnected){
         /* publish the message  */
         iMosqResult = mosquitto_publish(   pMosq,                     // active mosquitto instance
                                             NULL,                      // message id
                                             currentTopicName,  // topic to be published
                                             strlen(strPayload),     // size of the current data
                                             strPayload,    // payload data
                                             1,                         // QoS 0, 1 or 2
                                             false);                    // retain On/Off

         /* inform the user of the result of the publish mosquitto API call */
         switch( iMosqResult ){
            case MOSQ_ERR_SUCCESS:
               /* the topic could be published after an error */
               printf( "Topic: %s - was published again.\n",currentTopicName);
               break;
            case MOSQ_ERR_INVAL:
               /* there was an invalid parameter input in publish function */
               printf( "Topic: %s - invalid parameter on service level.\n",currentTopicName);
               break;
            case MOSQ_ERR_NOMEM:
               /* the library ran out of memory */
               printf( "Topic: %s - invalid out of memory on service level.\n",currentTopicName);
               break;
            case MOSQ_ERR_NO_CONN:
               /* no connection to broker */
               printf( "Topic: %s - connection to broker lost during publishing.\n",currentTopicName);
               break;
            case MOSQ_ERR_PROTOCOL:
               /* there was a protocol problem during transmission */
               printf( "Topic: %s - protocol error between client and broker.\n",currentTopicName);
               break;
            case MOSQ_ERR_PAYLOAD_SIZE:
               /* the length of the payload was to long */
               printf( "Topic: %s - the payload length is too large.\n",currentTopicName);
               break;
            case MOSQ_ERR_MALFORMED_UTF8:
               /* the topic is not UTF8 formated */
               printf( "Topic: %s - the topic is not valid UTF-8 formated.\n",currentTopicName);
               break;
            case MOSQ_ERR_QOS_NOT_SUPPORTED:
               /* the QoS is greater than supported by the broker */
               printf( "Topic: %s - the QoS is higher than the broker can accept.\n",currentTopicName);
               break;
            case MOSQ_ERR_OVERSIZE_PACKET:
               /* the packet is too large for the broker to process */
               printf( "Topic: %s - the packet is too large for the broker.\n",currentTopicName);
               break;
            default:
               printf( "Topic: %s - undefined error occurs.\n",currentTopicName);
         } // close switch - generate individual fault

         /* wait 5 s.. */
         sleep(5);

         /* check if we shall disconnect*/
         if(xSIGINT == true){
            printf("SIGINT received. Disconnect from the broker, shut down mosquitto and exit..\n");
            /* disconnect from broker */
            iMosqResult = mosquitto_disconnect(pMosq);

            printf("disconnect result is: 0x%x.",iMosqResult);

            /* wait 10s for mosquitto to raise the disconnect callback */
            printf("waiting for disconnect..\n");
            /* reset the timeout counter */
            uiWaitSeconds = 0;
            /* wait second-wise.. */
            while(uiWaitSeconds < 10){
               /* check if the mosquitto disconnect callback has cleared the global "connected" flag */
               if (xConnected == false) {
                  break;
               }
               sleep(1);
               /* count the seconds.. */
               uiWaitSeconds++;
            }
            /* force end of publish while loop */
            xConnected = false;

            printf("...disconnected.\n");

            /* force stop of mosquitto network communication loop. */
            printf("mosquitto_loop_stop()..\n");
            mosquitto_loop_stop(pMosq, true);

            /* destroy our mosquitto client instance. */
            printf( "destroy mosquitto client instance.\n");
            mosquitto_destroy(pMosq);

            /* leave this program */
            printf("exiting..\n");
         }
      }
   }
}
