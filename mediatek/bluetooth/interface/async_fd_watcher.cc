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

#define LOG_TAG "mtk.hal.bt@1.0-impl"

#include "async_fd_watcher.h"

#include <fcntl.h>
#include <sys/select.h>
#include <unistd.h>

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

#include <log/log.h>

static const int INVALID_FD = -1;

static const int BT_RT_PRIORITY = 1;

namespace android {
namespace hardware {
namespace bluetooth {
namespace async {

int AsyncFdWatcher::WatchFdForNonBlockingReads(
    int file_descriptor, const ReadCallback& on_read_fd_ready_callback) {
  // Add file descriptor and callback
  {
    std::unique_lock<std::mutex> guard(internal_mutex_);
    watched_fds_[file_descriptor] = on_read_fd_ready_callback;
  }

  // Start the thread if not started yet
  return tryStartThread();
}

int AsyncFdWatcher::ConfigureTimeout(
    const std::chrono::milliseconds timeout,
    const TimeoutCallback& on_timeout_callback) {
  // Add timeout and callback
  {
    std::unique_lock<std::mutex> guard(timeout_mutex_);
    timeout_cb_ = on_timeout_callback;
    timeout_ms_ = timeout;
  }

  notifyThread();
  return 0;
}

void AsyncFdWatcher::StopWatchingFileDescriptors() { stopThread(); }

AsyncFdWatcher::AsyncFdWatcher()
    : notification_listen_fd_(INVALID_FD),
      notification_write_fd_(INVALID_FD),
      timeout_cb_(nullptr),
      timeout_ms_(std::chrono::milliseconds(0)) {}

AsyncFdWatcher::~AsyncFdWatcher() {}

// Make sure to call this with at least one file descriptor ready to be
// watched upon or the thread routine will return immediately
int AsyncFdWatcher::tryStartThread() {
  if (std::atomic_exchange(&running_, true)) return 0;

  // Set up the communication channel
  int pipe_fds[2];
  if (pipe2(pipe_fds, O_NONBLOCK)) return -1;

  notification_listen_fd_ = pipe_fds[0];
  notification_write_fd_ = pipe_fds[1];

  thread_ = std::thread([this]() { ThreadRoutine(); });
  if (!thread_.joinable()) return -1;

  return 0;
}

int AsyncFdWatcher::stopThread() {
  if (!std::atomic_exchange(&running_, false)) return 0;

  notifyThread();
  if (std::this_thread::get_id() != thread_.get_id()) {
    thread_.join();
  }

  {
    std::unique_lock<std::mutex> guard(internal_mutex_);
    watched_fds_.clear();
  }

  {
    std::unique_lock<std::mutex> guard(timeout_mutex_);
    timeout_cb_ = nullptr;
  }

  close(notification_listen_fd_);
  close(notification_write_fd_);

  return 0;
}

int AsyncFdWatcher::notifyThread() {
  uint8_t buffer[] = {0};
  if (TEMP_FAILURE_RETRY(write(notification_write_fd_, &buffer, 1)) < 0) {
    return -1;
  }
  return 0;
}

void AsyncFdWatcher::ThreadRoutine() {
  // Make watching thread RT.
  struct sched_param rt_params;
  rt_params.sched_priority = BT_RT_PRIORITY;
  if (sched_setscheduler(gettid(), SCHED_FIFO, &rt_params)) {
    ALOGE("%s unable to set SCHED_FIFO for pid %d, tid %d, error %s", __func__,
          getpid(), gettid(), strerror(errno));
  }

  while (running_) {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(notification_listen_fd_, &read_fds);
    int max_read_fd = INVALID_FD;
    for (auto& it : watched_fds_) {
      FD_SET(it.first, &read_fds);
      max_read_fd = std::max(max_read_fd, it.first);
    }

    struct timeval timeout;
    struct timeval* timeout_ptr = NULL;
    if (timeout_ms_ > std::chrono::milliseconds(0)) {
      timeout.tv_sec = timeout_ms_.count() / 1000;
      timeout.tv_usec = (timeout_ms_.count() % 1000) * 1000;
      timeout_ptr = &timeout;
    }

    // Wait until there is data available to read on some FD.
    int nfds = std::max(notification_listen_fd_, max_read_fd);
    int retval = select(nfds + 1, &read_fds, NULL, NULL, timeout_ptr);

    // There was some error.
    if (retval < 0) continue;

    // Timeout.
    if (retval == 0) {
      // Allow the timeout callback to modify the timeout.
      TimeoutCallback saved_cb;
      {
        std::unique_lock<std::mutex> guard(timeout_mutex_);
        if (timeout_ms_ > std::chrono::milliseconds(0))
          saved_cb = timeout_cb_;
      }
      if (saved_cb != nullptr)
        saved_cb();
      continue;
    }

    // Read data from the notification FD.
    if (FD_ISSET(notification_listen_fd_, &read_fds)) {
      char buffer[] = {0};
      TEMP_FAILURE_RETRY(read(notification_listen_fd_, buffer, 1));
      continue;
    }

    // Invoke the data ready callbacks if appropriate.
    {
      // Hold the mutex to make sure that the callbacks are still valid.
      std::unique_lock<std::mutex> guard(internal_mutex_);
      for (auto& it : watched_fds_) {
        if (FD_ISSET(it.first, &read_fds)) {
        it.second(it.first);
        }
      }
    }
  }
}

} // namespace async
} // namespace bluetooth
} // namespace hardware
} // namespace android
