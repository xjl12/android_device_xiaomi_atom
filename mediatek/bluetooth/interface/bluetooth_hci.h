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

#ifndef HIDL_GENERATED_android_hardware_bluetooth_V1_0_BluetoothHci_H_
#define HIDL_GENERATED_android_hardware_bluetooth_V1_0_BluetoothHci_H_

#include <android/hardware/bluetooth/1.0/IBluetoothHci.h>

#include <hidl/MQDescriptor.h>

#include <functional>

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

using ::android::hardware::Return;
using ::android::hardware::hidl_vec;

class BluetoothDeathRecipient;

class BluetoothHci : public IBluetoothHci {
 public:
  BluetoothHci();
  Return<void> initialize(
      const ::android::sp<IBluetoothHciCallbacks>& cb) override;
  Return<void> sendHciCommand(const hidl_vec<uint8_t>& packet) override;
  Return<void> sendAclData(const hidl_vec<uint8_t>& data) override;
  Return<void> sendScoData(const hidl_vec<uint8_t>& data) override;
  Return<void> close() override;

 private:
  void sendDataToController(const uint8_t type, const hidl_vec<uint8_t>& data);
  ::android::sp<BluetoothDeathRecipient> death_recipient_;
  std::function<void(sp<BluetoothDeathRecipient>&)> unlink_cb_;
};

extern "C" IBluetoothHci* HIDL_FETCH_IBluetoothHci(const char* name);

}  // namespace implementation
}  // namespace V1_0
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_android_hardware_bluetooth_V1_0_BluetoothHci_H_
