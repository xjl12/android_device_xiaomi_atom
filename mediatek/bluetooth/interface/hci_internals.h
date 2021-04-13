//
// Copyright 2016 The Android Open Source Project
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

#include <stdlib.h>

// HCI UART transport packet types (Volume 4, Part A, 2)
enum HciPacketType {
  HCI_PACKET_TYPE_UNKNOWN = 0,
  HCI_PACKET_TYPE_COMMAND = 1,
  HCI_PACKET_TYPE_ACL_DATA = 2,
  HCI_PACKET_TYPE_SCO_DATA = 3,
  HCI_PACKET_TYPE_EVENT = 4
};

// 2 bytes for opcode, 1 byte for parameter length (Volume 2, Part E, 5.4.1)
const size_t HCI_COMMAND_PREAMBLE_SIZE = 3;
const size_t HCI_LENGTH_OFFSET_CMD = 2;

// 2 bytes for handle, 2 bytes for data length (Volume 2, Part E, 5.4.2)
const size_t HCI_ACL_PREAMBLE_SIZE = 4;
const size_t HCI_LENGTH_OFFSET_ACL = 2;

// 2 bytes for handle, 1 byte for data length (Volume 2, Part E, 5.4.3)
const size_t HCI_SCO_PREAMBLE_SIZE = 3;
const size_t HCI_LENGTH_OFFSET_SCO = 2;

// 1 byte for event code, 1 byte for parameter length (Volume 2, Part E, 5.4.4)
const size_t HCI_EVENT_PREAMBLE_SIZE = 2;
const size_t HCI_LENGTH_OFFSET_EVT = 1;

const size_t HCI_PREAMBLE_SIZE_MAX = HCI_ACL_PREAMBLE_SIZE;

// Event codes (Volume 2, Part E, 7.7.14)
const uint8_t HCI_COMMAND_COMPLETE_EVENT = 0x0E;
