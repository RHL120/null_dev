#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

int major = -1;

module_param(major, int, S_IRUGO);

struct cdev null_cdev;
struct cdev zero_cdev;

ssize_t null_write(struct file *filp, const char __user *buff, size_t size, loff_t *off)
{
	printk(KERN_ALERT "from null_write\n");
	return size;
}

ssize_t zero_read(struct file *filp, char __user *buff, size_t size, loff_t *off)
{
	char *zero = kmalloc(size * sizeof(char), GFP_KERNEL);
	ssize_t ret = -EFAULT;
	printk(KERN_ALERT "from zero_read\n");
	if (zero) {
		printk(KERN_ALERT "allocated zero\n");
		memset(zero, 0, size * sizeof(char));
		printk(KERN_ALERT "memset zero\n");
		if (!copy_to_user(buff, zero, size))
			ret = size;
		kfree(zero);
	}
	printk(KERN_ALERT "got res: %ld, %d\n", ret, -EFAULT);
	return ret;
}

struct file_operations null_fops = {
	.owner = THIS_MODULE,
	.write = null_write
};

struct file_operations zero_fops = {
	.owner = THIS_MODULE,
	.read = zero_read
};

int null_setup_cdev(void)
{
	dev_t num = MKDEV(major, 0);
	cdev_init(&null_cdev, &null_fops);
	null_cdev.owner = THIS_MODULE;
	return cdev_add(&null_cdev, num, 1);
}

int zero_setup_cdev(void)
{
	dev_t num = MKDEV(major, 1);
	cdev_init(&zero_cdev, &zero_fops);
	zero_cdev.owner = THIS_MODULE;
	return cdev_add(&zero_cdev, num, 1);
}

int nulld_init(void)
{
	int res = 0;
	dev_t dev;
	if (major < 0) {
		res = alloc_chrdev_region(&dev, 0, 2, "nulld");
		major = MAJOR(dev);
	} else {
		dev = MKDEV(major, 0);
		res = register_chrdev_region(dev, 2, "nulld");
	}
	if (res < 0) {
		printk(KERN_ERR "failed to get device number\n");
		goto ret;
	}
	printk(KERN_NOTICE "got major: %d", major);
	if ((res = null_setup_cdev()) < 0) {
		printk(KERN_ALERT "failed to setup null's cdev\n");
		goto null_cdev_ret;
	}
	if ((res = zero_setup_cdev()) < 0) {
		printk(KERN_ALERT "failed to setup zero's cdev\n");
		goto zero_cdev_ret;
	}
	printk(KERN_NOTICE "zero's and null's cdevs are allocated\n");
	goto ret;
zero_cdev_ret:
	cdev_del(&null_cdev);
null_cdev_ret:
	unregister_chrdev_region(dev, 2);

ret:
	return res;
}

void nulld_exit(void)
{
	dev_t num = MKDEV(major, 0);
	printk(KERN_NOTICE "cleaning up\n");
	cdev_del(&null_cdev);
	cdev_del(&zero_cdev);
	unregister_chrdev_region(num, 2);
}

module_init(nulld_init);
module_exit(nulld_exit);
