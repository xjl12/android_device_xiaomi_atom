/*
 * Copyright (c) 2012-2018, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * *    * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define LOG_NIDEBUG 0

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>

#define LOG_TAG "QTI PowerHAL"
#include <utils/Log.h>
#include <hardware/hardware.h>
#include <hardware/power.h>

#include "utils.h"
#include "hint-data.h"
#include "performance.h"
#include "power-common-old.h"

static struct hint_handles handles[NUM_HINTS];

static int power_device_open(const hw_module_t* module, const char* name,
        hw_device_t** device);

static struct hw_module_methods_t power_module_methods = {
    .open = power_device_open,
};

static void power_init(struct power_module *module)
{
    ALOGI("Initing");

    for (int i=0; i<NUM_HINTS; i++) {
        handles[i].handle       = 0;
        handles[i].ref_count    = 0;
    }
}

int __attribute__ ((weak)) power_hint_override(struct power_module *module, power_hint_t hint,
        void *data)
{
    return HINT_NONE;
}

/* Declare function before use */
void interaction(int duration, int num_args, int opt_list[]);

static void power_hint(struct power_module *module, power_hint_t hint,
        void *data)
{
    /* Check if this hint has been overridden. */
    if (power_hint_override(module, hint, data) == HINT_HANDLED) {
        /* The power_hint has been handled. We can skip the rest. */
        return;
    }
    switch(hint) {
        case POWER_HINT_VSYNC:
        break;
        case POWER_HINT_VR_MODE:
            ALOGI("VR mode power hint not handled in power_hint_override");
            break;
        case POWER_HINT_INTERACTION:
        {
            int resources[] = {0x702, 0x20F, 0x30F};
            int duration = 3000;

            interaction(duration, sizeof(resources)/sizeof(resources[0]), resources);
        }
        break;
        //fall through below, hints will fail if not defined in powerhint.xml
        case POWER_HINT_SUSTAINED_PERFORMANCE:
        case POWER_HINT_VIDEO_ENCODE:
            if (data) {
                if (handles[hint].ref_count == 0)
                    handles[hint].handle = perf_hint_enable((AOSP_DELTA + hint), 0);

                if (handles[hint].handle > 0)
                    handles[hint].ref_count++;
            }
            else
                if (handles[hint].handle > 0)
                    if (--handles[hint].ref_count == 0) {
                        release_request(handles[hint].handle);
                        handles[hint].handle = 0;
                    }
                else
                    ALOGE("Lock for hint: %X was not acquired, cannot be released", hint);
        break;
    }
}

int __attribute__ ((weak)) set_interactive_override(struct power_module *module, int on)
{
    return HINT_NONE;
}

void set_interactive(struct power_module *module, int on)
{
    if (!on) {
        /* Send Display OFF hint to perf HAL */
        perf_hint_enable(VENDOR_HINT_DISPLAY_OFF, 0);
    } else {
        /* Send Display ON hint to perf HAL */
        perf_hint_enable(VENDOR_HINT_DISPLAY_ON, 0);
    }

    if (set_interactive_override(module, on) == HINT_HANDLED) {
        return;
    }

    ALOGI("Got set_interactive hint");
}

static int power_device_open(const hw_module_t* module, const char* name,
        hw_device_t** device)
{
    int status = -EINVAL;
    if (module && name && device) {
        if (!strcmp(name, POWER_HARDWARE_MODULE_ID)) {
            power_module_t *dev = (power_module_t *)malloc(sizeof(*dev));

            if(dev) {
                memset(dev, 0, sizeof(*dev));

                if(dev) {
                    /* initialize the fields */
                    dev->common.module_api_version = POWER_MODULE_API_VERSION_0_2;
                    dev->common.tag = HARDWARE_DEVICE_TAG;
                    dev->init = power_init;
                    dev->powerHint = power_hint;
                    dev->setInteractive = set_interactive;
                    /* At the moment we support 0.2 APIs */
                    dev->setFeature = NULL,
                        dev->get_number_of_platform_modes = NULL,
                        dev->get_platform_low_power_stats = NULL,
                        dev->get_voter_list = NULL,
                        *device = (hw_device_t*)dev;
                    status = 0;
                } else {
                    status = -ENOMEM;
                }
            }
            else {
                status = -ENOMEM;
            }
        }
    }

    return status;
}

struct power_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = POWER_MODULE_API_VERSION_0_2,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = POWER_HARDWARE_MODULE_ID,
        .name = "QTI Power HAL",
        .author = "QTI",
        .methods = &power_module_methods,
    },

    .init = power_init,
    .powerHint = power_hint,
    .setInteractive = set_interactive,
};
