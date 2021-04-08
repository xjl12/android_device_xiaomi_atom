/*
 * Copyright (c) 2012, 2013, 2015, 2017-2018, The Linux Foundation. All rights reserved.
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

/* Default use-case hint IDs */
#define DEFAULT_VIDEO_ENCODE_HINT_ID    (0x0A00)
#define DEFAULT_VIDEO_DECODE_HINT_ID    (0x0B00)
#define DISPLAY_STATE_HINT_ID           (0x0C00)
#define DISPLAY_STATE_HINT_ID_2         (0x0D00)
#define CAM_PREVIEW_HINT_ID             (0x0E00)
#define SUSTAINED_PERF_HINT_ID          (0x0F00)
#define VR_MODE_HINT_ID                 (0x1000)
#define VR_MODE_SUSTAINED_PERF_HINT_ID  (0x1001)

#define AOSP_DELTA                      (0x1200)

#define VSYNC_HINT                      AOSP_DELTA + POWER_HINT_VSYNC
#define INTERACTION_HINT                AOSP_DELTA + POWER_HINT_INTERACTION
#define VIDEO_DECODE_HINT               AOSP_DELTA + POWER_HINT_VIDEO_DECODE
#define VIDEO_ENCODE_HINT               AOSP_DELTA + POWER_HINT_VIDEO_ENCODE
#define LOW_POWER_HINT                  AOSP_DELTA + POWER_HINT_LOW_POWER
#define SUSTAINED_PERF_HINT             AOSP_DELTA + POWER_HINT_SUSTAINED_PERFORMANCE
#define VR_MODE_HINT                    AOSP_DELTA + POWER_HINT_VR_MODE
#define LAUNCH_HINT                     AOSP_DELTA + POWER_HINT_LAUNCH
#define DISABLE_TOUCH_HINT              AOSP_DELTA + POWER_HINT_DISABLE_TOUCH

//update NUM_HINTS if hints are added to AOSP
#define NUM_HINTS                       (POWER_HINT_DISABLE_TOUCH +1)

#define VR_MODE_SUSTAINED_PERF_HINT    (0x1301)

struct hint_handles{
    int handle;
    int ref_count;
};

struct hint_data {
    unsigned long hint_id; /* This is our key. */
    unsigned long perflock_handle;
};

int hint_compare(struct hint_data *first_hint,
        struct hint_data *other_hint);
void hint_dump(struct hint_data *hint);
