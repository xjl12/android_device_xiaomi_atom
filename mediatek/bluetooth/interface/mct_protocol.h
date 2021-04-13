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

#include "async_fd_watcher.h"
#include "bt_vendor_lib.h"
#include "hci_internals.h"
#include "hci_protocol.h"

namespace android {
namespace hardware {
namespace bluetooth {
namespace hci {

class MctProtocol : public HciProtocol {
 public:
  MctProtocol(int* fds, PacketReadCallback event_cb, PacketReadCallback acl_cb);

  size_t Send(uint8_t type, const uint8_t* data, size_t length);

  void OnEventPacketReady();
  void OnAclDataPacketReady();

  void OnEventDataReady(int fd);
  void OnAclDataReady(int fd);

 private:
  int uart_fds_[CH_MAX];

  PacketReadCallback event_cb_;
  PacketReadCallback acl_cb_;

  hci::HciPacketizer event_packetizer_;
  hci::HciPacketizer acl_packetizer_;
};

}  // namespace hci
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
