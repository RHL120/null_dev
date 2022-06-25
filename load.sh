#!/bin/sh
insmod $* || exit 1
major=$(awk "\$2==\"nulld\" {print \$1}" /proc/devices)
mknod /dev/nulld_null c $major 0
mknod /dev/nulld_zero c $major 1
chmod 444 /dev/nulld_zero
chmod 222 /dev/nulld_null