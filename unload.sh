#!/bin/sh
if [ -z $NULLD_REPEAT_PATH ]
then
	NULLD_REPEAT_PATH="/dev/nulld_zero"
fi

if [ -z $NULLD_HOLE_PATH ]
then
	NULLD_HOLE_PATH="/dev/nulld_null"
fi
rmmod nulld
rm $NULLD_HOLE_PATH
rm $NULLD_REPEAT_PATH
