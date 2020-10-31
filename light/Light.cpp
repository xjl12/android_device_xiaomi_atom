/*
 * Copyright (C) 2018 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "LightService"

#include <log/log.h>

#include "Light.h"

#include <fstream>

#define LCD_LED               "/sys/class/backlight/panel0-backlight/"
#define BLUE_LED              "/sys/class/leds/blue/"

#define BREATH                "breath"
#define BRIGHTNESS            "brightness"
#define DELAY_OFF             "delay_off"
#define DELAY_ON              "delay_on"

#define MAX_LED_BRIGHTNESS    255
#define MAX_LCD_BRIGHTNESS    2047

namespace {
/*
 * Write value to path and close file.
 */
static void set(std::string path, std::string value) {
    std::ofstream file(path);

    if (!file.is_open()) {
        ALOGW("failed to write %s to %s", value.c_str(), path.c_str());
        return;
    }

    file << value;
}

static void set(std::string path, int value) {
    set(path, std::to_string(value));
}

static uint32_t getBrightness(const LightState& state) {
    uint32_t alpha, red, green, blue;

    /*
     * Extract brightness from AARRGGBB.
     */
    alpha = (state.color >> 24) & 0xFF;
    red = (state.color >> 16) & 0xFF;
    green = (state.color >> 8) & 0xFF;
    blue = state.color & 0xFF;

    /*
     * Scale RGB brightness if Alpha brightness is not 0xFF.
     */
    if (alpha != 0xFF) {
        red = red * alpha / 0xFF;
        green = green * alpha / 0xFF;
        blue = blue * alpha / 0xFF;
    }

    return (77 * red + 150 * green + 29 * blue) >> 8;
}

static inline uint32_t scaleBrightness(uint32_t brightness, uint32_t maxBrightness) {
    return brightness * maxBrightness / 0xFF;
}

static inline uint32_t getScaledBrightness(const LightState& state, uint32_t maxBrightness) {
    return scaleBrightness(getBrightness(state), maxBrightness);
}

static void handleBacklight(Type /* type */, const LightState& state) {
    uint32_t brightness = getScaledBrightness(state, MAX_LCD_BRIGHTNESS);
    set(LCD_LED BRIGHTNESS, brightness);
}

static void setNotification(const LightState& state) {
    uint32_t blueBrightness = getScaledBrightness(state, MAX_LED_BRIGHTNESS);

    /* Disable breathing */
    set(BLUE_LED BREATH, 0);

    if (state.flashMode == Flash::TIMED) {
        /* Enable breathing */
        set(BLUE_LED BREATH, 1);
        set(BLUE_LED DELAY_OFF, state.flashOnMs);
	    set(BLUE_LED DELAY_ON, state.flashOffMs);
    } else {
        set(BLUE_LED BRIGHTNESS, blueBrightness);
    }
}

static inline bool isLit(const LightState& state) {
    return state.color & 0x00ffffff;
}

/*
 * Keep sorted in the order of importance.
 */
static const LightState offState = {};
static std::vector<std::pair<Type, LightState>> notificationStates = {
    { Type::ATTENTION, offState },
    { Type::NOTIFICATIONS, offState },
    { Type::BATTERY, offState },
};

static void handleNotification(Type type, const LightState& state) {
    bool handled = false;

    for(auto it : notificationStates) {
        if (it.first == type) {
            it.second = state;
        }

        if  (!handled && isLit(it.second)) {
            setNotification(it.second);
            handled = true;
        }
    }

    if (!handled) {
        setNotification(offState);
    }
}

static std::map<Type, std::function<void(Type type, const LightState&)>> lights = {
    { Type::ATTENTION, handleNotification },
    { Type::NOTIFICATIONS, handleNotification },
    { Type::BATTERY, handleNotification },
    { Type::BACKLIGHT, handleBacklight },
};

}  // anonymous namespace

namespace android {
namespace hardware {
namespace light {
namespace V2_0 {
namespace implementation {

Return<Status> Light::setLight(Type type, const LightState& state) {
    auto it = lights.find(type);

    if (it == lights.end()) {
        return Status::LIGHT_NOT_SUPPORTED;
    }

    /*
     * Lock global mutex until light state is updated.
     */
    std::lock_guard<std::mutex> lock(globalLock);

    it->second(type, state);

    return Status::SUCCESS;
}

Return<void> Light::getSupportedTypes(getSupportedTypes_cb _hidl_cb) {
    std::vector<Type> types;

    for (auto const& light : lights) {
        types.push_back(light.first);
    }

    _hidl_cb(types);

    return Void();
}

}  // namespace implementation
}  // namespace V2_0
}  // namespace light
}  // namespace hardware
}  // namespace android
