//
// Copyright 2017 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#include <hidl/HidlSupport.h>

#include <sys/uio.h>

#include "bt_vendor_lib.h"
#include "hci_internals.h"
#include "hci_packetizer.h"

namespace android {
namespace hardware {
namespace bluetooth {
namespace hci {

using ::android::hardware::hidl_vec;
using PacketReadCallback = std::function<void(const hidl_vec<uint8_t>&)>;

// Implementation of HCI protocol bits common to different transports
class HciProtocol {
 public:
  HciProtocol() = default;
  virtual ~HciProtocol(){};

  // Protocol-specific implementation of sending packets.
  virtual size_t Send(uint8_t type, const uint8_t* data, size_t length) = 0;

 protected:
  static size_t WriteSafely(int fd, const uint8_t* data, size_t length);
  static size_t WritevSafely(int fd, struct iovec* iov, int iovcnt);
};

}  // namespace hci
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
