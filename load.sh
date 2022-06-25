#!/bin/sh
insmod $* || exit 1
major=$(awk "\$2==\"nulld\" {print \$1}" /proc/devices)
mknod /dev/nulld_null c $major 0
mknod /dev/nulld_zero c $major 1
mknod /dev/nulld_all c $major 2
chmod 222 /dev/nulld_null
chmod 444 /dev/nulld_zero
chmod 666 /dev/nulld_all
