#include "nulld.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>

const char *DPATH = "/dev/nulld_zero";
const char *REPEAT_ENV = "NULLD_REPEAT_PATH";

int load_dev()
{
	char *env = getenv(REPEAT_ENV);
	return env? open(env, 0): open(DPATH, 0);
}

int main(int argc, char *argv[])
{
	int ret = 0;
	if (argc < 2) {
		puts("Usage: srp <get | set <repeat>>");
		return -1;
	}
	if (!strcmp(argv[1], "help")) {
		puts("srp: Get or set the value repeated by nulld_zero");
		puts("actions:");
		printf("get%16cGet's the current repeated value\n", ' ');
		printf("set%4c<repeat>%4csets the repeated value to repeat\n",
				' ', ' ');
		printf("help%15cprints this message\n", ' ');
		return 0;
	}
	int dev_fd = load_dev();
	if (dev_fd < 0) {
		puts("Failed to open device");
		return -1;
	}
	if (!strcmp(argv[1], "get")) {
		char repeat;
		if (ioctl(dev_fd, NULLD_GET_REPEAT, &repeat)) {
			puts("Failed to get repeat");
			ret = -1;
		} else {
			printf("Repeat is 0x%x\n", repeat);
		}
	} else if (!strcmp(argv[1], "set")) {
		if (argc < 3) {
			puts("Usage: srp set <repeat>");
			ret = -1;
		} else {
			long val = strtol(argv[2], NULL, 0);
			if (val > -128 && val < 127) {
				char cval = (char) val;
				if (ioctl(dev_fd, NULLD_SET_REPEAT, &cval)) {
					puts("Failed to change repeat");
				}
			} else {
				puts("repeat should be between -128 and 127");
				ret = -1;
			}
		}
	} else {
		printf("Action %s is not vaild\n", argv[1]);
	}
	close(dev_fd);
	return ret;
}
