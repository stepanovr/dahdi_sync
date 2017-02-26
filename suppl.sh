#!/bin/bash -p

ROOT=
TMPFILE=/tmp/tmp.$$
INITD=/etc/init.d
MODULES_FILE=/etc/dahdi/modules
INIT_FILE=/etc/dahdi/init.conf
TARGET="/lib/modules/$(uname -r)/extra"

if [ -d /etc/rc.d/init.d ]; then
	INITD=/etc/rc.d/init.d
elif [ -d  /etc/init.d ]; then
	INITD=/etc/init.d
else
	echo "Error: No /etc/init.d directory found!"
	exit 1
fi

err=1;
SNAME="dahdi"
CNAME="Dahdi"


if [ ! -f sync_timer.ko ]
then
	echo "Error: build driver first"
	exit 1
else
	if [ -d "$TARGET" ]
	then
		cp sync_timer.ko $TARGET
	else
		echo "Error: No driver directory"
	fi
fi


# Just add a couple of strings to dahdi/modules:
if [ -f $MODULES_FILE ]; then
	sed -i '/# Xmobex Timer Sync Driver/d' $MODULES_FILE
	sed -i '/sync_timer/d' $MODULES_FILE
#	echo  >> $MODULES_FILE
	echo "# Xmobex Timer Sync Driver" >> $MODULES_FILE
        echo "sync_timer" >> $MODULES_FILE
else
	echo  "No DAHDI file $MODULES_FILE found"
	exit 1
fi

if [ -f $INIT_FILE ]; then
	./driver_uninst.sh $INIT_FILE
else
        sed -i '/# Xmobex Timer Sync Driver/d' $MODULES_FILE
        sed -i '/sync_timer/d' $MODULES_FILE
        echo  "No DAHDI file $INIT_FILE found"
fi



echo "--------------------------------------------------------------------------"
echo " Sync Timer driver Boot Setup Complete!"
echo "Do this operation every time after adding new dahdi module to the system"
echo "--------------------------------------------------------------------------"
echo "The sync_timer module was added to $MODULES_FILE and $INIT_FILE lists."
echo
echo "Simply start $SNAME using: $INITD/$SNAME start or service dahdi start"
echo "--------------------------------------------------------------------------"

exit 0

