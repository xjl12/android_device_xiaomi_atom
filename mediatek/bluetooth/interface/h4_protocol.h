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

class H4Protocol : public HciProtocol {
 public:
  H4Protocol(int fd, PacketReadCallback event_cb, PacketReadCallback acl_cb,
             PacketReadCallback sco_cb)
      : uart_fd_(fd),
        event_cb_(event_cb),
        acl_cb_(acl_cb),
        sco_cb_(sco_cb),
        hci_packetizer_([this]() { OnPacketReady(); }) {}

  size_t Send(uint8_t type, const uint8_t* data, size_t length);

  void OnPacketReady();

  void OnDataReady(int fd);

 private:
  int uart_fd_;

  PacketReadCallback event_cb_;
  PacketReadCallback acl_cb_;
  PacketReadCallback sco_cb_;

  HciPacketType hci_packet_type_{HCI_PACKET_TYPE_UNKNOWN};
  hci::HciPacketizer hci_packetizer_;
};

}  // namespace hci
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
