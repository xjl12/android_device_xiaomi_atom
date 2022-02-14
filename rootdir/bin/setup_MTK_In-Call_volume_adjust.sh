#!/bin/sh
### Begonia In-call volume fix by TTTT555 ###
while [ "$(getprop sys.boot_completed | tr -d '\r')" != "1" ]; do sleep 1; done
sleep 10

# channel current volume (to find all params use "tinymix" command)
chcurrent=$(/system/bin/tinymix "ADDA_DL_GAIN" --v)
# used gain = chcurrent + our offset < max value by tinymix "ADDA_DL_GAIN"
chused=65535
# choose your min variant FIXME: maybe need rewrite script for using our min variant
#chmin=99
# start value need less than $callmin
callcurrentold=0

# write logs
/system/bin/log -t MTKInCallDirtyFix "Starting Service"
#/system/bin/log -t MTKInCallDirtyFix "AudioManager Min Volume: $callmin"
#/system/bin/log -t MTKInCallDirtyFix "AudioManager Max Volume: $callmax"
#/system/bin/log -t MTKInCallDirtyFix "AudioManager Current Volume: $callcurrent"
#/system/bin/log -t MTKInCallDirtyFix "Hardware Min Volume: $chmin"
/system/bin/log -t MTKInCallDirtyFix "Hardware Current Gain: $chcurrent"
/system/bin/log -t MTKInCallDirtyFix "Hardware Used Gain: $chused"
#/system/bin/log -t MTKInCallDirtyFix "Hardware Volume Interval: $interval"

# loop
# run every 2 seconds
while true
do
# check call status
  callstatus=$(/system/bin/tinymix "Speech_SCP_CALL_STATE" --v)

  if [ "$callstatus" = 1 ]
    then
  # check call min volume in decimal FIXME: write HEX to DEX in one line
    callmin=$(/system/bin/service call audio 17 i32 0 | /system/bin/awk -F ' ' '{print $3}' | sed 's/0*//')
    callmin=$(echo "$((16#$callmin))")
  # check call max volume in decimal FIXME: write HEX to DEX in one line
    callmax=$(/system/bin/service call audio 18 i32 0 | /system/bin/awk -F ' ' '{print $3}' | sed 's/0*//')
    callmax=$(echo "$((16#$callmax))")
  # calculate interval for using via parabolic function
    interval=$(/system/bin/expr $chused / $callmax / $callmax)

      # enable deep buffer for fix crackling
      /system/bin/tinymix "deep_buffer_irq_cnt" 2048
      /system/bin/tinymix "deep_buffer_scenario" 1

	while [ "$callstatus" = 1 ]
	do
      # check choosen volume level FIXME: write HEX to DEX in one line
      callcurrent=$(/system/bin/service call audio 16 i32 0 | /system/bin/awk -F ' ' '{print $3}' | sed 's/0*//')
      callcurrent=$(echo "$((16#$callcurrent))")
      if [ "$callcurrentold" != "$callcurrent" ]
	    then
          # calculate and write the new channel volume via parabolic function & rewrite max Db for small speaker
          newchvol=$(/system/bin/expr $callcurrent \* $callcurrent \* $interval)
          /system/bin/tinymix "ADDA_DL_GAIN" $newchvol
	  /system/bin/tinymix "Handset_PGA_GAIN" 8Db
          /system/bin/log -t MTKInCallDirtyFix "Hardware In-Call Volume: $newchvol"

	  callcurrentold=$callcurrent
      fi
	  callstatus=$(/system/bin/tinymix "Speech_SCP_CALL_STATE" --v)
	  done
     #restore $callcurrentold and chcurrent
       /system/bin/tinymix "ADDA_DL_GAIN" $chcurrent
       callcurrentold=0
  fi
sleep 2

done