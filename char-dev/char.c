#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "chardev"
static int major_number;
static char msg[256] = {0};

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset){
    return simple_read_from_buffer(buffer, len, offset, msg, sizeof(msg));
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    if (copy_from_user(msg, buffer, len)) {
        return -EFAULT;
    }
    return len;
}


static struct file_operations fops =
{
    .read = dev_read,
    .write = dev_write,
};


static int __init char_init(void) {
    major_number = register_chrdev(0, DEVICE_NAME, &fops);

    if(major_number < 0){
        printk(KERN_ALERT "Char device registration failed with %d\n", major_number);
        return major_number;
    }

    printk(KERN_INFO "Char device registered with major number %d\n", major_number);
    return 0;
}

static void __exit char_exit(void) {
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Char device unregistered\n");
}

module_init(char_init);
module_exit(char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eeshan");
MODULE_DESCRIPTION("Basic Character driver");