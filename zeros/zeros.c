#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define DEVICE_NAME "zeros"
#define CLASS_NAME "zeros_class"

static int major;
static struct class *zeros_class = NULL;

static int zeros_open(struct inode *inode, struct file *file) {
	pr_info("%s: Device opened\n", DEVICE_NAME);
	return 0;
}

static int zeros_release(struct inode *inode, struct file *file) {
	pr_info("%s: Device closed\n", DEVICE_NAME);
	return 0;
}

static ssize_t zeros_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
	char *kbuf;
	size_t i;

	kbuf = kmalloc(count, GFP_KERNEL);
	if (!kbuf)
		return -ENOMEM;

	for(i = 0; i < count; i++){
		kbuf[i] = '0';
	}

	if (copy_to_user(buf, kbuf, count)) {
		kfree(kbuf);
		return -EFAULT;
	}

	kfree(kbuf);
	return count;
}

static struct file_operations zeros_fops = {
	.owner = THIS_MODULE,
	.open = zeros_open,
	.release = zeros_release,
	.read = zeros_read,
};

static int __init zeros_init(void) {
	major = register_chrdev(0, DEVICE_NAME, &zeros_fops);
	if (major < 0) {
		pr_err("%s: Failed to register device\n", DEVICE_NAME);
		return major;
	}
	
	pr_info("%s: Device registered with major number %d\n", DEVICE_NAME, major);

	zeros_class = class_create(CLASS_NAME);
	if (IS_ERR(zeros_class)) {
		unregister_chrdev(major, DEVICE_NAME);
		pr_err("%s: Failed to create device class\n", DEVICE_NAME);
		return PTR_ERR(zeros_class);
	}

	if(!device_create(zeros_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME)) {
		class_destroy(zeros_class);
		unregister_chrdev(major, DEVICE_NAME);
		pr_err("%s: Failed to create device node\n", DEVICE_NAME);
		return -1;
	}

	pr_info("%s: Device initialized successfully\n", DEVICE_NAME);
	return 0;
}

static void __exit zeros_exit(void) {
	device_destroy(zeros_class, MKDEV(major, 0));
	class_destroy(zeros_class);
	unregister_chrdev(major, DEVICE_NAME);
	pr_info("%s: Device unregistered\n", DEVICE_NAME);
}

module_init(zeros_init);
module_exit(zeros_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eeshan");
MODULE_DESCRIPTION("Driver to replicate /dev/zero but output '0' strings.");
