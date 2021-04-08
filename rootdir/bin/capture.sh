# $1: audio source
#     main-mic: main mic
#     top-mic: top mic
#     back-mic: back mic
#     front-mic: front mic
# $2: sample rate(Hz)
# $3: sample bit
# $4: channel number
# $5: capture duration(s)
# tinycap file.wav [-D card] [-d device] [-c channels] [-r rate] [-b bits] [-p period_size] [-n n_periods] [-t duration]
# sample usage: capture.sh main-mic 48000 16 2 1.5
# sample usage: capture.sh top-mic 48000 16 2 1.5

rate=KHZ_48
filename=/sdcard/unknown_mic.wav

case "$2" in
    "48000" )
        rate=KHZ_48
        ;;
    "96000" )
        rate=KHZ_96
        ;;
    "192000" )
        rate=KHZ_192
        ;;
esac

function enable_main_mic
{
    echo "enabling main mic"
    tinymix 'Mic_Type_Mux_0' 'DCC'
    tinymix 'UL1_CH1 ADDA_UL_CH1' 1
    tinymix 'UL1_CH2 ADDA_UL_CH2' 1
    tinymix 'MISO0_MUX' 'UL1_CH1'
    tinymix 'MISO1_MUX' 'UL1_CH1'
    tinymix 'ADC_L_Mux' 'Left Preamplifier'
    tinymix 'PGA_L_Mux' 'AIN0'
}

function disable_main_mic
{
    echo "disabling main mic"
    tinymix 'Mic_Type_Mux_0' 'Idle'
    tinymix 'UL1_CH1 ADDA_UL_CH1' 0
    tinymix 'UL1_CH2 ADDA_UL_CH2' 0
    tinymix 'ADC_L_Mux' 'Idle'
    tinymix 'ADC_R_Mux' 'Idle'
    tinymix 'PGA_L_Mux' 'None'
    tinymix 'PGA_R_Mux' 'None'
}

function enable_top_mic
{
    echo "enabling top mic"
    tinymix 'Mic_Type_Mux_2' 'DCC'
    tinymix 'UL1_CH1 ADDA_UL_CH1' 1
    tinymix 'UL1_CH2 ADDA_UL_CH2' 1
    tinymix 'MISO0_MUX' 'UL1_CH2'
    tinymix 'MISO1_MUX' 'UL1_CH2'
    tinymix 'ADC_R_Mux' 'Right Preamplifier'
    tinymix 'PGA_R_Mux' 'AIN2'
}

function disable_top_mic
{
    echo "disabling top mic"
    tinymix 'Mic_Type_Mux_2' 'Idle'
    tinymix 'UL1_CH1 ADDA_UL_CH1' 0
    tinymix 'UL1_CH2 ADDA_UL_CH2' 0
    tinymix 'ADC_L_Mux' 'Idle'
    tinymix 'ADC_R_Mux' 'Idle'
    tinymix 'PGA_L_Mux' 'None'
    tinymix 'PGA_R_Mux' 'None'
}

function enable_back_mic
{
    echo "enabling back mic"
    tinymix 'Mic_Type_Mux_2' 'DCC'
    tinymix 'UL1_CH1 ADDA_UL_CH1' 1
    tinymix 'UL1_CH2 ADDA_UL_CH2' 1
    tinymix 'MISO0_MUX' 'UL1_CH2'
    tinymix 'MISO1_MUX' 'UL1_CH2'
    tinymix 'ADC_R_Mux' 'Right Preamplifier'
    tinymix 'PGA_R_Mux' 'AIN3'
}

function disable_back_mic
{
    echo "disabling back mic"
    tinymix 'Mic_Type_Mux_2' 'Idle'
    tinymix 'UL1_CH1 ADDA_UL_CH1' 0
    tinymix 'UL1_CH2 ADDA_UL_CH2' 0
    tinymix 'ADC_L_Mux' 'Idle'
    tinymix 'ADC_R_Mux' 'Idle'
    tinymix 'PGA_L_Mux' 'None'
    tinymix 'PGA_R_Mux' 'None'
}

function enable_front_mic
{
    echo "PHONE HAS NO FRONT MIC!"
}

function disable_front_mic
{
    echo "PHONE HAS NO FRONT MIC!"
}

function enable_ultrasound_mic
{
    echo "DOSE NOT SUPPORT ULTRASOUND FEATURE"
}

function disable_ultrasound_mic
{
    echo "DOSE NOT SUPPORT ULTRASOUND FEATURE"
}

case "$1" in
    "main-mic" )
        enable_main_mic
        filename=/sdcard/main_mic.wav
        ;;
    "top-mic" )
        enable_top_mic
        filename=/sdcard/top_mic.wav
        ;;
    "back-mic" )
        enable_back_mic
        filename=/sdcard/back_mic.wav
        ;;
    "front-mic" )
        enable_front_mic
        filename=/sdcard/front_mic.wav
        ;;
    "us" )
        enable_ultrasound_mic
        filename=/sdcard/us_mic.wav
        ;;
    *)
        echo "Usage: capture.sh main-mic 48000 16 2 10"
        ;;
esac

if [ -z "$6" ]; then
    period_size=1024
else
    period_size=$6
fi

if [ -z "$7" ]; then
    n_periods=4
else
    n_periods=$7
fi


# start recording
echo "start recording"
tinycap $filename -D 0 -d 10 -r $2 -b $3 -c $4 -T $5 -p $period_size -n $n_periods
ret=$?
if [ $ret -ne 0 ]; then
    echo "tinycap done, return $ret"
fi

# tear down
case "$1" in
    "main-mic" )
        disable_main_mic
        ;;
    "top-mic" )
        disable_top_mic
        ;;
    "back-mic" )
        disable_back_mic
        ;;
    "front-mic" )
        disable_front_mic
        ;;
    "us" )
        disable_ultrasound_mic
        ;;
esac
