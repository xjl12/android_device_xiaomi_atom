AudioSetParam SET_LOOPBACK_TYPE=0

sleep 0.3
tinymix 'Headset_PGAL_GAIN' 8
tinymix 'Headset_PGAR_GAIN' 8
tinymix 'Audio_PGA1_Setting' 3
tinymix 'Audio_PGA2_Setting' 3

sleep 0.3
AudioSetParam SET_AURISYS_ON=1
sleep 0.3