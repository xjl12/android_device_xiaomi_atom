/*
 * Copyright (C) 2014, 2017-2018 The  Linux Foundation. All rights reserved.
 * Not a contribution
 * Copyright (C) 2008 The Android Open Source Project
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


// #define LOG_NDEBUG 0

#include <log/log.h>
#include <cutils/properties.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#include <sys/ioctl.h>
#include <sys/types.h>

#include <hardware/lights.h>

#ifndef DEFAULT_LOW_PERSISTENCE_MODE_BRIGHTNESS
#define DEFAULT_LOW_PERSISTENCE_MODE_BRIGHTNESS 0x80
#endif

/******************************************************************************/

static pthread_once_t g_init = PTHREAD_ONCE_INIT;
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
static struct light_state_t g_notification;
static struct light_state_t g_battery;
static int g_last_backlight_mode = BRIGHTNESS_MODE_USER;
static int g_attention = 0;
static bool g_has_persistence_node = false;

char const*const LCD_FILE
        = "/sys/class/leds/lcd-backlight/brightness";

char const*const LCD_FILE2
        = "/sys/class/backlight/panel0-backlight/brightness";

char const*const BUTTON_FILE
        = "/sys/class/leds/button-backlight/brightness";

char const*const PERSISTENCE_FILE
        = "/sys/class/graphics/fb0/msm_fb_persist_mode";

enum rgb_led {
    LED_RED = 0,
    LED_GREEN,
    LED_BLUE,
};

char *led_names[] = {
    "red",
    "green",
    "blue",
};
/**
 * device methods
 */

void init_globals(void)
{
    // init the mutex
    pthread_mutex_init(&g_lock, NULL);
}

static int write_int(char const* path, int value)
{
    int fd;
    static int already_warned = 0;

    fd = open(path, O_RDWR);
    if (fd >= 0) {
        char buffer[20];
        int bytes = snprintf(buffer, sizeof(buffer), "%d\n", value);
        ssize_t amt = write(fd, buffer, (size_t)bytes);
        close(fd);
        return amt == -1 ? -errno : 0;
    } else {
        if (already_warned == 0) {
            ALOGE("write_int failed to open %s, errno = %d\n", path, errno);
            already_warned = 1;
        }
        return -errno;
    }
}

static bool file_exists(const char *file)
{
    int fd;

    fd = open(file, O_RDWR);
    if (fd < 0) {
        ALOGE("failed to open %s, errno=%d\n", file, errno);
        return false;
    }

    close(fd);
    return true;
}

static int
is_lit(struct light_state_t const* state)
{
    return state->color & 0x00ffffff;
}

static int
rgb_to_brightness(struct light_state_t const* state)
{
    int color = state->color & 0x00ffffff;
    return ((77*((color>>16)&0x00ff))
            + (150*((color>>8)&0x00ff)) + (29*(color&0x00ff))) >> 8;
}

static int
set_light_backlight(struct light_device_t* dev,
        struct light_state_t const* state)
{
    int err = 0;
    int brightness = rgb_to_brightness(state);
    unsigned int lpEnabled =
        state->brightnessMode == BRIGHTNESS_MODE_LOW_PERSISTENCE;
    if(!dev) {
        return -1;
    }

    pthread_mutex_lock(&g_lock);
    // Toggle low persistence mode state
    bool persistence_mode = ((g_last_backlight_mode != state->brightnessMode && lpEnabled) ||
                            (!lpEnabled &&
                            g_last_backlight_mode == BRIGHTNESS_MODE_LOW_PERSISTENCE));
    bool cannot_handle_persistence = !g_has_persistence_node && persistence_mode;
    if (g_has_persistence_node) {
        if (persistence_mode) {
            if ((err = write_int(PERSISTENCE_FILE, lpEnabled)) != 0) {
                ALOGE("%s: Failed to write to %s: %s\n", __FUNCTION__,
                       PERSISTENCE_FILE, strerror(errno));
            }
            if (lpEnabled != 0) {
                brightness = DEFAULT_LOW_PERSISTENCE_MODE_BRIGHTNESS;
            }
        }
        g_last_backlight_mode = state->brightnessMode;
    }

    if (!err) {
        if (!access(LCD_FILE, F_OK)) {
            err = write_int(LCD_FILE, brightness);
        } else {
            err = write_int(LCD_FILE2, brightness);
        }
    }

    pthread_mutex_unlock(&g_lock);
    return cannot_handle_persistence ? -ENOSYS : err;
}

static int set_rgb_led_brightness(enum rgb_led led, int brightness)
{
    char file[48];

    snprintf(file, sizeof(file), "/sys/class/leds/%s/brightness", led_names[led]);
    return write_int(file, brightness);
}

static int set_rgb_led_timer_trigger(enum rgb_led led, int onMS, int offMS)
{
    char file[48];
    int rc;

    snprintf(file, sizeof(file), "/sys/class/leds/%s/delay_off", led_names[led]);
    rc = write_int(file, offMS);
    if (rc < 0)
        goto out;

    snprintf(file, sizeof(file), "/sys/class/leds/%s/delay_on", led_names[led]);
    rc = write_int(file, onMS);
    if (rc < 0)
        goto out;

    return 0;
out:
    ALOGD("%s doesn't support timer trigger\n", led_names[led]);
    return rc;
}

static int set_rgb_led_hw_blink(enum rgb_led led, int blink)
{
    char file[48];

    snprintf(file, sizeof(file), "/sys/class/leds/%s/breath", led_names[led]);
    if (!file_exists(file))
        snprintf(file, sizeof(file), "/sys/class/leds/%s/blink", led_names[led]);

    return write_int(file, blink);
}

static int
set_speaker_light_locked(struct light_device_t* dev,
        struct light_state_t const* state)
{
    int red, green, blue;
    int onMS, offMS;
    unsigned int colorRGB;
    int blink = 0;
    int rc = 0;

    if(!dev) {
        return -1;
    }

    colorRGB = state->color;
    red = (colorRGB >> 16) & 0xFF;
    green = (colorRGB >> 8) & 0xFF;
    blue = colorRGB & 0xFF;

    onMS = state->flashOnMS;
    offMS = state->flashOffMS;

    if (onMS != 0 && offMS != 0)
        blink = 1;

    switch (state->flashMode) {
        case LIGHT_FLASH_HARDWARE:
            if (!!red)
                rc = set_rgb_led_hw_blink(LED_RED, blink);
            if (!!green)
                rc |= set_rgb_led_hw_blink(LED_GREEN, blink);
            if (!!blue)
                rc |= set_rgb_led_hw_blink(LED_BLUE, blink);
            /* fallback to timed blinking if breath is not supported */
            if (rc == 0)
                break;
        case LIGHT_FLASH_TIMED:
            if (!!red)
                rc = set_rgb_led_timer_trigger(LED_RED, onMS, offMS);
            if (!!green)
                rc |= set_rgb_led_timer_trigger(LED_GREEN, onMS, offMS);
            if (!!blue)
                rc |= set_rgb_led_timer_trigger(LED_BLUE, onMS, offMS);
            /* fallback to constant on if timed blinking is not supported */
            if (rc == 0)
                break;
        case LIGHT_FLASH_NONE:
        default:
            rc = set_rgb_led_brightness(LED_RED, red);
            rc |= set_rgb_led_brightness(LED_GREEN, green);
            rc |= set_rgb_led_brightness(LED_BLUE, blue);
            break;
    }

    ALOGD("set_speaker_light_locked mode=%d, colorRGB=%08X, onMS=%d, offMS=%d, rc=%d\n",
            state->flashMode, colorRGB, onMS, offMS, rc);

    return rc;
}

static void
handle_speaker_battery_locked(struct light_device_t* dev)
{
    if (is_lit(&g_battery)) {
        set_speaker_light_locked(dev, &g_battery);
    } else {
        set_speaker_light_locked(dev, &g_notification);
    }
}

static int
set_light_battery(struct light_device_t* dev,
        struct light_state_t const* state)
{
    pthread_mutex_lock(&g_lock);
    g_battery = *state;
    handle_speaker_battery_locked(dev);
    pthread_mutex_unlock(&g_lock);
    return 0;
}

static int
set_light_notifications(struct light_device_t* dev,
        struct light_state_t const* state)
{
    pthread_mutex_lock(&g_lock);
    g_notification = *state;
    handle_speaker_battery_locked(dev);
    pthread_mutex_unlock(&g_lock);
    return 0;
}

static int
set_light_attention(struct light_device_t* dev,
        struct light_state_t const* state)
{
    pthread_mutex_lock(&g_lock);
    if (state->flashMode == LIGHT_FLASH_HARDWARE) {
        g_attention = state->flashOnMS;
    } else if (state->flashMode == LIGHT_FLASH_NONE) {
        g_attention = 0;
    }
    handle_speaker_battery_locked(dev);
    pthread_mutex_unlock(&g_lock);
    return 0;
}

static int
set_light_buttons(struct light_device_t* dev,
        struct light_state_t const* state)
{
    int err = 0;
    if(!dev) {
        return -1;
    }
    pthread_mutex_lock(&g_lock);
    err = write_int(BUTTON_FILE, state->color & 0xFF);
    pthread_mutex_unlock(&g_lock);
    return err;
}

/** Close the lights device */
static int
close_lights(struct light_device_t *dev)
{
    if (dev) {
        free(dev);
    }
    return 0;
}


/******************************************************************************/

/**
 * module methods
 */

/** Open a new instance of a lights device using name */
static int open_lights(const struct hw_module_t* module, char const* name,
        struct hw_device_t** device)
{
    int (*set_light)(struct light_device_t* dev,
            struct light_state_t const* state);

    if (0 == strcmp(LIGHT_ID_BACKLIGHT, name)) {
        g_has_persistence_node = !access(PERSISTENCE_FILE, F_OK);
        set_light = set_light_backlight;
    } else if (0 == strcmp(LIGHT_ID_BATTERY, name))
        set_light = set_light_battery;
    else if (0 == strcmp(LIGHT_ID_NOTIFICATIONS, name))
        set_light = set_light_notifications;
    else if (0 == strcmp(LIGHT_ID_BUTTONS, name)) {
        if (!access(BUTTON_FILE, F_OK)) {
          // enable light button when the file is present
          set_light = set_light_buttons;
        } else {
          return -EINVAL;
        }
    }
    else if (0 == strcmp(LIGHT_ID_ATTENTION, name))
        set_light = set_light_attention;
    else
        return -EINVAL;

    pthread_once(&g_init, init_globals);

    struct light_device_t *dev = malloc(sizeof(struct light_device_t));

    if(!dev)
        return -ENOMEM;

    memset(dev, 0, sizeof(*dev));

    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = LIGHTS_DEVICE_API_VERSION_2_0;
    dev->common.module = (struct hw_module_t*)module;
    dev->common.close = (int (*)(struct hw_device_t*))close_lights;
    dev->set_light = set_light;

    *device = (struct hw_device_t*)dev;
    return 0;
}

static struct hw_module_methods_t lights_module_methods = {
    .open =  open_lights,
};

/*
 * The lights Module
 */
struct hw_module_t HAL_MODULE_INFO_SYM = {
    .tag = HARDWARE_MODULE_TAG,
    .version_major = 1,
    .version_minor = 0,
    .id = LIGHTS_HARDWARE_MODULE_ID,
    .name = "lights Module",
    .author = "Google, Inc.",
    .methods = &lights_module_methods,
};
