// Copyright 2023 Weidmueller Interface GmbH & Co. KG <oss@weidmueller.com>
//
// SPDX-License-Identifier: Apache-2.0

#include "iostream"
#include <iomanip>

void PrintBuffer(uint8_t* buf, int size) {
  std::cout << std::hex << std::setfill('0');  // needs to be set only once

  using u64 = unsigned long long;

  for (int i = 0; i < size; i++, buf++) {
    if (i % sizeof(u64) == 0) {
      std::cout << std::endl;
    }
    std::cout << std::setw(2) << static_cast<unsigned>(*buf) << " ";
  }
  std::cout << std::endl;
  std::cout << std::endl;
}
