#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include "nulld.h"

MODULE_LICENSE("GPL");

int major = 0;
int repeat = 0;
module_param(repeat, int, S_IRUGO);
module_param(major, int, S_IRUGO);

struct hole_device {
	struct cdev cdev;
};

struct repeat_device {
	char repeat;
	struct cdev cdev;
};

struct hole_device hole_dev;
struct repeat_device repeat_dev;

int repeat_open(struct inode *node, struct file *filp)
{
	filp->private_data = container_of(node->i_cdev, struct repeat_device,
			cdev);
	return 0;
}
ssize_t repeat_read(struct file *f, char __user *b, size_t size, loff_t *off)
{
	struct repeat_device *rd = f->private_data;
	// Store in local variable in case rd->repeat changes.
	char rp = rd->repeat;
	for (size_t i = 0; i < size; i++) {
		if (put_user(rp, b + i))
			return -EFAULT;
	}
	return size;
}

long repeat_ioctl(struct file *filp, unsigned int cmd, unsigned long a)
{
	struct repeat_device *rd = filp->private_data;
	char *arg = (char *) a;
	switch (cmd) {
	case NULLD_SET_REPEAT:
		return get_user(rd->repeat, arg);
	case NULLD_GET_REPEAT:
		return put_user(rd->repeat, arg);
	}
	return -ENOTTY;
}

struct file_operations repeat_fops = {
	.unlocked_ioctl = repeat_ioctl,
	.open = repeat_open,
	.read = repeat_read

};

int repeat_setup(struct repeat_device *rd, int major, int minor, char rp)
{
	rd->repeat = rp;
	cdev_init(&rd->cdev, &repeat_fops);
	return cdev_add(&rd->cdev, MKDEV(major, minor), 1);

}

ssize_t hole_write(struct file *f, const char __user *b, size_t s, loff_t *o)
{
	return s;
}

struct file_operations hole_fops = {
	.write = hole_write
};

int hole_setup(struct hole_device *hd, int major, int minor)
{
	cdev_init(&hd->cdev, &hole_fops);
	return cdev_add(&hd->cdev, MKDEV(major, minor), 1);

}

int nulld_init(void)
{
	int res = 0;
	if (major) {
		res = register_chrdev_region(MKDEV(major, 0), 2, "nulld");
	} else {
		dev_t num;
		res = alloc_chrdev_region(&num, 0, 2, "nulld");
		major = MAJOR(num);
	}
	if (res < 0) {
		printk(KERN_ERR "Failed to allocate chrdev region\n");
		goto ret;
	}
	printk(KERN_NOTICE "Got Major number: %d\n", major);
	if ((res = hole_setup(&hole_dev, major, 0)) < 0) {
		printk(KERN_ERR "Failed to setup the hole\n");
		goto hole_fail;
	}
	if ((res = repeat_setup(&repeat_dev, major, 1, repeat)) < 0) {
		printk(KERN_ERR "Failed to setup repeat\n");
		goto repeat_fail;
	}
	goto ret;
repeat_fail: cdev_del(&hole_dev.cdev);
hole_fail: unregister_chrdev_region(MKDEV(major, 0), 2);
ret: return res;
}

void nulld_exit(void)
{
	cdev_del(&repeat_dev.cdev);
	cdev_del(&hole_dev.cdev);
	unregister_chrdev_region(MKDEV(major, 0), 2);
}

module_init(nulld_init);
module_exit(nulld_exit);
