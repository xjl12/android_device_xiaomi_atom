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

#include <functional>

#include <hidl/HidlSupport.h>

#include "hci_internals.h"

namespace android {
namespace hardware {
namespace bluetooth {
namespace hci {

using ::android::hardware::hidl_vec;
using HciPacketReadyCallback = std::function<void(void)>;

class HciPacketizer {
 public:
  HciPacketizer(HciPacketReadyCallback packet_cb)
      : packet_ready_cb_(packet_cb){};
  void OnDataReady(int fd, HciPacketType packet_type);
  const hidl_vec<uint8_t>& GetPacket() const;

 protected:
  enum State { HCI_PREAMBLE, HCI_PAYLOAD };
  State state_{HCI_PREAMBLE};
  uint8_t preamble_[HCI_PREAMBLE_SIZE_MAX];
  hidl_vec<uint8_t> packet_;
  size_t bytes_remaining_{0};
  size_t bytes_read_{0};
  HciPacketReadyCallback packet_ready_cb_;
};

}  // namespace hci
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
