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

#include "h4_protocol.h"

#define LOG_TAG "mtk.hal.bt-hci-h4"

#include <fcntl.h>
#include <unistd.h>

#include <log/log.h>

namespace android {
namespace hardware {
namespace bluetooth {
namespace hci {

size_t H4Protocol::Send(uint8_t type, const uint8_t* data, size_t length) {
  std::unique_ptr<uint8_t[]> adapted_data(new uint8_t[length+1]);
  uint8_t* p_data = adapted_data.get();
  *(p_data) = type;
  memcpy(++p_data, data, length);
  int rv = WriteSafely(uart_fd_, --p_data, length+1);
  return rv;
}

void H4Protocol::OnPacketReady() {
  switch (hci_packet_type_) {
    case HCI_PACKET_TYPE_EVENT:
      event_cb_(hci_packetizer_.GetPacket());
      break;
    case HCI_PACKET_TYPE_ACL_DATA:
      acl_cb_(hci_packetizer_.GetPacket());
      break;
    case HCI_PACKET_TYPE_SCO_DATA:
      sco_cb_(hci_packetizer_.GetPacket());
      break;
    default:
      LOG_ALWAYS_FATAL("%s: Unimplemented packet type %d", __func__,
                       static_cast<int>(hci_packet_type_));
  }
  // Get ready for the next type byte.
  hci_packet_type_ = HCI_PACKET_TYPE_UNKNOWN;
}

void H4Protocol::OnDataReady(int fd) {
  if (hci_packet_type_ == HCI_PACKET_TYPE_UNKNOWN) {
    uint8_t buffer[1] = {0};
    ssize_t bytes_read = TEMP_FAILURE_RETRY(read(fd, buffer, 1));
    if (bytes_read != 1) {
      if (bytes_read == 0) {
        LOG_ALWAYS_FATAL("%s: Unexpected EOF reading the packet type!",
                         __func__);
      } else if (bytes_read < 0) {
        LOG_ALWAYS_FATAL("%s: Read packet type error: %s", __func__,
                         strerror(errno));
      } else {
        LOG_ALWAYS_FATAL("%s: More bytes read than expected (%u)!", __func__,
                         static_cast<unsigned int>(bytes_read));
      }
    }
    hci_packet_type_ = static_cast<HciPacketType>(buffer[0]);
    if (hci_packet_type_ != HCI_PACKET_TYPE_ACL_DATA &&
        hci_packet_type_ != HCI_PACKET_TYPE_SCO_DATA &&
        hci_packet_type_ != HCI_PACKET_TYPE_EVENT) {
      LOG_ALWAYS_FATAL("%s: Unimplemented packet type %d", __func__,
                       static_cast<int>(hci_packet_type_));
    }
  } else {
    hci_packetizer_.OnDataReady(fd, hci_packet_type_);
  }
}

}  // namespace hci
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
