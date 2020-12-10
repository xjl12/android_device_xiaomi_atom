/* Copyright (c) 2012, 2014, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
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
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#define FAILED                  -1
#define SUCCESS                 0
#define INDEFINITE_DURATION     0

/* Hints sent to perf HAL from power HAL
 * These have to be kept in sync with Perf HAL side definitions
 */
#define VENDOR_HINT_DISPLAY_OFF      0x00001040
#define VENDOR_HINT_DISPLAY_ON       0x00001041

enum SCREEN_DISPLAY_TYPE {
    DISPLAY_OFF = 0x00FF,
};

enum PWR_CLSP_TYPE {
    ALL_CPUS_PWR_CLPS_DIS = 0x101,
};

/* For CPUx min freq, the leftmost byte
 * represents the CPU and the
 * rightmost byte represents the frequency
 * All intermediate frequencies on the
 * device are supported. The hex value
 * passed into PerfLock will be multiplied
 * by 10^5. This frequency or the next
 * highest frequency available will be set
 *
 * For example, if 1.4 Ghz is required on
 * CPU0, use 0x20E
 *
 * If the highest available frequency
 * on the device is required, use
 * CPUx_MIN_FREQ_TURBO_MAX
 * where x represents the CPU
 */
enum CPU0_MIN_FREQ_LVL {
    CPU0_MIN_FREQ_NONTURBO_MAX = 0x20A,
    CPU0_MIN_FREQ_TURBO_MAX = 0x2FE,
};

enum CPU1_MIN_FREQ_LVL {
    CPU1_MIN_FREQ_NONTURBO_MAX = 0x30A,
    CPU1_MIN_FREQ_TURBO_MAX = 0x3FE,
};

enum CPU2_MIN_FREQ_LVL {
    CPU2_MIN_FREQ_NONTURBO_MAX = 0x40A,
    CPU2_MIN_FREQ_TURBO_MAX = 0x4FE,
};

enum CPU3_MIN_FREQ_LVL {
    CPU3_MIN_FREQ_NONTURBO_MAX = 0x50A,
    CPU3_MIN_FREQ_TURBO_MAX = 0x5FE,
};

enum CPU0_MAX_FREQ_LVL {
    CPU0_MAX_FREQ_NONTURBO_MAX = 0x150A,
};

enum CPU1_MAX_FREQ_LVL {
    CPU1_MAX_FREQ_NONTURBO_MAX = 0x160A,
};

enum CPU2_MAX_FREQ_LVL {
    CPU2_MAX_FREQ_NONTURBO_MAX = 0x170A,
};

enum CPU3_MAX_FREQ_LVL {
    CPU3_MAX_FREQ_NONTURBO_MAX = 0x180A,
};

enum MIN_CPUS_ONLINE_LVL {
    CPUS_ONLINE_MIN_2 = 0x702,
    CPUS_ONLINE_MIN_3 = 0x703,
    CPUS_ONLINE_MIN_4 = 0x704,
    CPUS_ONLINE_MPD_OVERRIDE = 0x777,
    CPUS_ONLINE_MAX = 0x7FF,
};

enum MAX_CPUS_ONLINE_LVL {
    CPUS_ONLINE_MAX_LIMIT_1 = 0x8FE,
    CPUS_ONLINE_MAX_LIMIT_2 = 0x8FD,
    CPUS_ONLINE_MAX_LIMIT_3 = 0x8FC,
    CPUS_ONLINE_MAX_LIMIT_4 = 0x8FB,
    CPUS_ONLINE_MAX_LIMIT_MAX = 0x8FB,
};

enum SAMPLING_RATE_LVL {
    MS_500 = 0xBCD,
    MS_50 = 0xBFA,
    MS_20 = 0xBFD,
};

enum ONDEMAND_IO_BUSY_LVL {
    IO_BUSY_OFF = 0xC00,
    IO_BUSY_ON = 0xC01,
};

enum ONDEMAND_SAMPLING_DOWN_FACTOR_LVL {
    SAMPLING_DOWN_FACTOR_1 = 0xD01,
    SAMPLING_DOWN_FACTOR_4 = 0xD04,
};

enum INTERACTIVE_TIMER_RATE_LVL {
    TR_MS_500 = 0xECD,
    TR_MS_100 = 0xEF5,
    TR_MS_50 = 0xEFA,
    TR_MS_30 = 0xEFC,
    TR_MS_20 = 0xEFD,
};

/* This timer rate applicable to cpu0
    across 8939 series chipset */
enum INTERACTIVE_TIMER_RATE_LVL_CPU0_8939 {
    TR_MS_CPU0_500 = 0x30CD,
    TR_MS_CPU0_100 = 0x30F5,
    TR_MS_CPU0_50 = 0x30FA,
    TR_MS_CPU0_30 = 0x30FC,
    TR_MS_CPU0_20 = 0x30FD,
};

/* This timer rate applicable to cpu4
    across 8939 series chipset */
enum INTERACTIVE_TIMER_RATE_LVL_CPU4_8939 {
    TR_MS_CPU4_500 = 0x3BCD,
    TR_MS_CPU4_100 = 0x3BF5,
    TR_MS_CPU4_50 = 0x3BFA,
    TR_MS_CPU4_30 = 0x3BFC,
    TR_MS_CPU4_20 = 0x3BFD,
};

/* This timer rate applicable to big.little arch */
enum INTERACTIVE_TIMER_RATE_LVL_BIG_LITTLE {
    BIG_LITTLE_TR_MS_100 = 0x64,
    BIG_LITTLE_TR_MS_50 = 0x32,
    BIG_LITTLE_TR_MS_40 = 0x28,
    BIG_LITTLE_TR_MS_30 = 0x1E,
    BIG_LITTLE_TR_MS_20 = 0x14,
};

/* INTERACTIVE opcodes */
enum INTERACTIVE_OPCODES {
    INT_OP_CLUSTER0_TIMER_RATE = 0x41424000,
    INT_OP_CLUSTER1_TIMER_RATE = 0x41424100,
    INT_OP_CLUSTER0_USE_SCHED_LOAD = 0x41430000,
    INT_OP_CLUSTER1_USE_SCHED_LOAD = 0x41430100,
    INT_OP_CLUSTER0_USE_MIGRATION_NOTIF = 0x41434000,
    INT_OP_CLUSTER1_USE_MIGRATION_NOTIF = 0x41434100,
    INT_OP_NOTIFY_ON_MIGRATE = 0x4241C000
};

enum INTERACTIVE_HISPEED_FREQ_LVL {
    HS_FREQ_1026 = 0xF0A,
};

enum INTERACTIVE_HISPEED_LOAD_LVL {
    HISPEED_LOAD_90 = 0x105A,
};

enum SYNC_FREQ_LVL {
    SYNC_FREQ_300 = 0x1103,
    SYNC_FREQ_600 = 0X1106,
    SYNC_FREQ_384 = 0x1103,
    SYNC_FREQ_NONTURBO_MAX = 0x110A,
    SYNC_FREQ_TURBO = 0x110F,
};

enum OPTIMAL_FREQ_LVL {
    OPTIMAL_FREQ_300 = 0x1203,
    OPTIMAL_FREQ_600 = 0x1206,
    OPTIMAL_FREQ_384 = 0x1203,
    OPTIMAL_FREQ_NONTURBO_MAX = 0x120A,
    OPTIMAL_FREQ_TURBO = 0x120F,
};

enum SCREEN_PWR_CLPS_LVL {
    PWR_CLPS_DIS = 0x1300,
    PWR_CLPS_ENA = 0x1301,
};

enum THREAD_MIGRATION_LVL {
    THREAD_MIGRATION_SYNC_OFF = 0x1400,
};

enum INTERACTIVE_IO_BUSY_LVL {
    INTERACTIVE_IO_BUSY_OFF = 0x1B00,
    INTERACTIVE_IO_BUSY_ON = 0x1B01,
};

enum SCHED_BOOST_LVL {
    SCHED_BOOST_ON = 0x1E01,
};

enum CPU4_MIN_FREQ_LVL {
    CPU4_MIN_FREQ_NONTURBO_MAX = 0x1F0A,
    CPU4_MIN_FREQ_TURBO_MAX = 0x1FFE,
};

enum CPU5_MIN_FREQ_LVL {
    CPU5_MIN_FREQ_NONTURBO_MAX = 0x200A,
    CPU5_MIN_FREQ_TURBO_MAX = 0x20FE,
};

enum CPU6_MIN_FREQ_LVL {
    CPU6_MIN_FREQ_NONTURBO_MAX = 0x210A,
    CPU6_MIN_FREQ_TURBO_MAX = 0x21FE,
};

enum CPU7_MIN_FREQ_LVL {
    CPU7_MIN_FREQ_NONTURBO_MAX = 0x220A,
    CPU7_MIN_FREQ_TURBO_MAX = 0x22FE,
};

enum CPU4_MAX_FREQ_LVL {
    CPU4_MAX_FREQ_NONTURBO_MAX = 0x230A,
};

enum CPU5_MAX_FREQ_LVL {
    CPU5_MAX_FREQ_NONTURBO_MAX = 0x240A,
};

enum CPU6_MAX_FREQ_LVL {
    CPU6_MAX_FREQ_NONTURBO_MAX = 0x250A,
};

enum CPU7_MAX_FREQ_LVL {
    CPU7_MAX_FREQ_NONTURBO_MAX = 0x260A,
};

#ifdef __cplusplus
}
#endif
