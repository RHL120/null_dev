#include <linux/init.h>
#include <linux/fs.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

int major = -1;

module_param(major, int, S_IRUGO);

int nulld_init(void)
{
	int res = 0;
	if (major < 0) {
		dev_t dev_num;
		res = alloc_chrdev_region(&dev_num, 0, 2, "nulld");
		major = MAJOR(dev_num);
	} else {
		dev_t dev = MKDEV(major, 0);
		res = register_chrdev_region(dev, 2, "nulld");
	}
	if (res < 0) {
		printk(KERN_ERR "failed to get device number\n");
	} else {
		printk(KERN_NOTICE "got major: %d", major);
	}
	return res;
}

void nulld_exit(void)
{
	dev_t num = MKDEV(major, 0);
	printk(KERN_NOTICE "cleaning up\n");
	unregister_chrdev_region(num, 2);
}

module_init(nulld_init);
module_exit(nulld_exit);
