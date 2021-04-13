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

#include "mct_protocol.h"

#define LOG_TAG "mtk.hal.bt-hci-mct"

#include <fcntl.h>

#include <log/log.h>

namespace android {
namespace hardware {
namespace bluetooth {
namespace hci {

MctProtocol::MctProtocol(int* fds, PacketReadCallback event_cb,
                         PacketReadCallback acl_cb)
    : event_cb_(event_cb),
      acl_cb_(acl_cb),
      event_packetizer_([this]() { OnEventPacketReady(); }),
      acl_packetizer_([this]() { OnAclDataPacketReady(); }) {
  for (int i = 0; i < CH_MAX; i++) {
    uart_fds_[i] = fds[i];
  }
}

size_t MctProtocol::Send(uint8_t type, const uint8_t* data, size_t length) {
  if (type == HCI_PACKET_TYPE_COMMAND)
    return WriteSafely(uart_fds_[CH_CMD], data, length);
  if (type == HCI_PACKET_TYPE_ACL_DATA)
    return WriteSafely(uart_fds_[CH_ACL_OUT], data, length);
  LOG_ALWAYS_FATAL("%s: Unimplemented packet type = %d", __func__, type);
  return 0;
}

void MctProtocol::OnEventPacketReady() {
  event_cb_(event_packetizer_.GetPacket());
}

void MctProtocol::OnAclDataPacketReady() {
  acl_cb_(acl_packetizer_.GetPacket());
}

void MctProtocol::OnEventDataReady(int fd) {
  event_packetizer_.OnDataReady(fd, HCI_PACKET_TYPE_EVENT);
}

void MctProtocol::OnAclDataReady(int fd) {
  acl_packetizer_.OnDataReady(fd, HCI_PACKET_TYPE_ACL_DATA);
}

}  // namespace hci
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
