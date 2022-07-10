#include <linux/ioctl.h>
#ifndef NULLD_MAGIC
#define NULLD_MAGIC 'n'
#define NULLD_SET_REPEAT _IOW(NULLD_MAGIC, 0, char)
#define NULLD_GET_REPEAT _IOR(NULLD_MAGIC, 1, char)
#endif
