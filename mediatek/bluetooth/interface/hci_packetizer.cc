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

#include "hci_packetizer.h"

#define LOG_TAG "mtk.hal.bt.hci_packetizer"

#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <log/log.h>

namespace {

const size_t preamble_size_for_type[] = {
    0, HCI_COMMAND_PREAMBLE_SIZE, HCI_ACL_PREAMBLE_SIZE, HCI_SCO_PREAMBLE_SIZE,
    HCI_EVENT_PREAMBLE_SIZE};
const size_t packet_length_offset_for_type[] = {
    0, HCI_LENGTH_OFFSET_CMD, HCI_LENGTH_OFFSET_ACL, HCI_LENGTH_OFFSET_SCO,
    HCI_LENGTH_OFFSET_EVT};

size_t HciGetPacketLengthForType(HciPacketType type, const uint8_t* preamble) {
  size_t offset = packet_length_offset_for_type[type];
  if (type != HCI_PACKET_TYPE_ACL_DATA) return preamble[offset];
  return (((preamble[offset + 1]) << 8) | preamble[offset]);
}

}  // namespace

namespace android {
namespace hardware {
namespace bluetooth {
namespace hci {

const hidl_vec<uint8_t>& HciPacketizer::GetPacket() const {
  return packet_;
}

void HciPacketizer::OnDataReady(int fd, HciPacketType packet_type) {
  switch (state_) {
    case HCI_PREAMBLE: {
      ssize_t bytes_read = TEMP_FAILURE_RETRY(
          read(fd, preamble_ + bytes_read_,
               preamble_size_for_type[packet_type] - bytes_read_));
      if (bytes_read <= 0) {
        LOG_ALWAYS_FATAL_IF((bytes_read == 0),
                            "%s: Unexpected EOF reading the header!", __func__);
        LOG_ALWAYS_FATAL("%s: Read header error: %s", __func__,
                         strerror(errno));
      }
      bytes_read_ += bytes_read;
      if (bytes_read_ == preamble_size_for_type[packet_type]) {
        size_t packet_length =
            HciGetPacketLengthForType(packet_type, preamble_);
        packet_.resize(preamble_size_for_type[packet_type] + packet_length);
        memcpy(packet_.data(), preamble_, preamble_size_for_type[packet_type]);
        bytes_remaining_ = packet_length;
        state_ = HCI_PAYLOAD;
        bytes_read_ = 0;
      }
      break;
    }

    case HCI_PAYLOAD: {
      ssize_t bytes_read = TEMP_FAILURE_RETRY(read(
          fd,
          packet_.data() + preamble_size_for_type[packet_type] + bytes_read_,
          bytes_remaining_));
      if (bytes_read <= 0) {
        LOG_ALWAYS_FATAL_IF((bytes_read == 0),
                            "%s: Unexpected EOF reading the payload!",
                            __func__);
        LOG_ALWAYS_FATAL("%s: Read payload error: %s", __func__,
                         strerror(errno));
      }
      bytes_remaining_ -= bytes_read;
      bytes_read_ += bytes_read;
      if (bytes_remaining_ == 0) {
        packet_ready_cb_();
        state_ = HCI_PREAMBLE;
        bytes_read_ = 0;
      }
      break;
    }
  }
}

}  // namespace hci
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
