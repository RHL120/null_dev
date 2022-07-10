#!/bin/bash
insmod $* || exit 1
major=$(awk "\$2==\"nulld\" {print \$1}" /proc/devices)
if [ -z $NULLD_REPEAT_PATH ]
then
	NULLD_REPEAT_PATH="/dev/nulld_repeat"
fi

if [ -z $NULLD_HOLE_PATH ]
then
	NULLD_HOLE_PATH="/dev/nulld_null"
fi
mknod $NULLD_HOLE_PATH c $major 0
mknod $NULLD_REPEAT_PATH c $major 1
chmod 222 $NULLD_HOLE_PATH
chmod 444 $NULLD_REPEAT_PATH
