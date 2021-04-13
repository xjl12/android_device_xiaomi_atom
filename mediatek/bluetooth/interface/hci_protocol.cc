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

#include "hci_protocol.h"

#define LOG_TAG "mtk.hal.bt-hci-hci_protocol"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <thread>

#include <log/log.h>

namespace android {
namespace hardware {
namespace bluetooth {
namespace hci {

size_t HciProtocol::WriteSafely(int fd, const uint8_t* data, size_t length) {
  size_t transmitted_length = 0;
  while (length > 0) {
    ssize_t ret =
        TEMP_FAILURE_RETRY(write(fd, data + transmitted_length, length));

    if (ret == -1) {
      if (errno == EAGAIN) {
        // According to requirement from driver, it has to do delay before retry right away here
        // std::this_thread::yield() is suggested to use to avoid thread busy writing
        std::this_thread::yield();
        continue;
      }
      ALOGE("%s error writing to UART (%s)", __func__, strerror(errno));
      break;

    } else if (ret == 0) {
      // Nothing written :(
      ALOGE("%s zero bytes written - something went wrong...", __func__);
      break;
    }

    transmitted_length += ret;
    length -= ret;
  }

  return transmitted_length;
}

size_t HciProtocol::WritevSafely(int fd, struct iovec* iov, int iovcnt) {
  LOG_ALWAYS_FATAL_IF((-1 == fd), "%s: invalid fd!", __func__);
  LOG_ALWAYS_FATAL_IF((NULL == iov), "%s: invalid fd!", __func__);

  size_t transmitted_length = 0;
  while (iovcnt > 0) {
    ssize_t ret =
        TEMP_FAILURE_RETRY(writev(fd, iov, iovcnt));

    if (ret == -1) {
      if (errno == EAGAIN) {
        // According to requirement from driver, it has to do delay before retry right away here
        // std::this_thread::yield() is suggested to use to avoid thread busy writing
        std::this_thread::yield();
        continue;
      }
      ALOGE("%s error writing to UART (%s)", __func__, strerror(errno));
      break;
    } else if (ret == 0) {
      // Nothing written :(
      ALOGE("%s zero bytes written - something went wrong...", __func__);
      break;
    }

    transmitted_length += ret;

    while (static_cast<size_t>(ret) >= iov->iov_len) {
      // consume entire iovec
      ret -= iov->iov_len;
      --iovcnt;
      ++iov;
    }

    if (ret > 0) {
      // consume partial iovec
      iov->iov_len -= ret;
      iov->iov_base = static_cast<uint8_t*>(iov->iov_base) + ret;
    }
  }

  return transmitted_length;
}

}  // namespace hci
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
