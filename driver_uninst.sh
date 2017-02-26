#!/bin/bash
#CONF_FILE="/etc/dahdi/init.conf"
CONF_FILE=$1

STR=`grep DAHDI_UNLOAD_MODULES $CONF_FILE | grep -v \s*#`
STR1=`echo $STR | grep sync_timer`

#exit 0

#grep DAHDI_UNLOAD_MODULES $CONF_FILE | grep -v \s*# | grep  sync_timer

#echo
#echo $STR
#STR=${STR/\"\s*$/\

#STR=$(sed 's/\"\s*$//' <<< $STR)

#echo $STR
#echo
#echo $STR1
#echo


if [ ! -z "$STR1" ]
then
	exit 0
else
	if [ ! -z "$STR" ]
	then
		sed -i "s/^[ \t]*DAHDI_UNLOAD_MODULES/DAHDI_UNLOAD_MODULES/" $CONF_FILE
		STR=`grep DAHDI_UNLOAD_MODULES $CONF_FILE | grep -v \s*#`
		STR=$(sed 's/\"\s*$//' <<< $STR)
		sed -i "s/^$STR/$STR\ sync_timer/" $CONF_FILE
	else
		echo "# Xmobex Timer Sync Driver" >> $CONF_FILE
		echo "DAHDI_UNLOAD_MODULES=\"sync_timer\"" >> $CONF_FILE
	fi
fi

