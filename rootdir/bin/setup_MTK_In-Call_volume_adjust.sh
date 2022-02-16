#!/bin/sh
### Begonia In-call volume fix by TTTT555 ###
### Modified by xjl12 : Support handfree volume change ###
while [ "$(getprop sys.boot_completed | tr -d '\r')" != "1" ]; do sleep 1; done
sleep 10

# channel current volume (to find all params use "tinymix" command)
chcurrent=$(/system/bin/tinymix "ADDA_DL_GAIN" --v)
# used gain = chcurrent + our offset < max value by tinymix "ADDA_DL_GAIN"
chused=65535
# 817 is catched in Redmi 10X 5G, It maybe not suitable for every device
speaker_max=817
newchvol_speaker=$speaker_max
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
      voice_channel=$(/system/bin/tinymix "ADDA_DL_CH1 PCM_2_CAP_CH1" --v)
      if [ $voice_channel == "Off" ]
      then
        speaker_cur=$(/system/bin/tinymix "Digital PCM Volume" --v)
        if [ $speaker_cur != $newchvol_speaker ]
        then
          /system/bin/tinymix "Digital PCM Volume" $newchvol_speaker
        fi
      fi
      if [ "$callcurrentold" != "$callcurrent" ]
	    then
          # calculate and write the new channel volume via parabolic function & rewrite max Db for small speaker
          newchvol=$(/system/bin/expr $callcurrent \* $callcurrent \* $interval)
          # log_val = ln( e - 1 + $callcurrent / 5 )
          case $callcurrent in 
          1 )
            log_val=651
            ;;
          2 )
            log_val=751
            ;;
          3 )
            log_val=841
            ;;
          4 )
            log_val=924
            ;;
          * )
            log_val=1000
            ;;
          esac
          newchvol_speaker=$(/system/bin/expr $log_val \* $speaker_max / 1000)
          if [ $voice_channel == "On" ]
          then
            /system/bin/tinymix "ADDA_DL_GAIN" $newchvol
            /system/bin/log -t MTKInCallDirtyFix "Headset!"
          else
            /system/bin/tinymix "Digital PCM Volume" $newchvol_speaker
            /system/bin/log -t MTKInCallDirtyFix "Speaker!"
          fi
	        /system/bin/tinymix "Handset_PGA_GAIN" 8Db
          /system/bin/log -t MTKInCallDirtyFix "Hardware In-Call Volume: $newchvol and $newchvol_speaker"

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