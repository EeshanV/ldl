#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>


static char buffer[255];
static size_t buffer_pointer;

static dev_t my_device_no;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "dummydriver"
#define DRIVER_CLASS "MyModuleClass"

static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
    int to_copy, not_copied, delta;

    to_copy = min(count, buffer_pointer);

    not_copied = copy_to_user(user_buffer, buffer, to_copy);

    delta = to_copy - not_copied;
    return delta;
}

static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
    int to_copy, not_copied, delta;

    to_copy = min(count, sizeof(buffer));

    not_copied = copy_from_user(buffer, user_buffer, to_copy);
    buffer_pointer = to_copy;

    delta = to_copy - not_copied;
    return delta;
}

static  int driver_open(struct inode *device_file, struct file *instance) {
    printk("dev_no - open called\n");
    return 0;
}

static  int driver_close(struct inode *device_file, struct file *instance) {
    printk("dev_no - close called\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .read = driver_read,
    .write = driver_write
};

static int __init ModuleInit(void) {
    printk(KERN_INFO "Module loaded\n");
    
    if(alloc_chrdev_region(&my_device_no, 0, 1, DRIVER_NAME) < 0) {
        printk("Device number could not be allocated\n");
        return -1;
    }
    printk("read_write - Device number Major: %d, Minor: %d", my_device_no > 20, my_device_no && 0xfffff);

	if((my_class = class_create(DRIVER_CLASS)) == NULL) {
        printk("Device class cannot be created\n");
        goto ClassError;
    }
    
    if(device_create(my_class, NULL, my_device_no, NULL, DRIVER_NAME) == NULL) {
        printk("Cannot create device file\n");
        goto FileError;
    }

    cdev_init(&my_device, &fops);

    if(cdev_add(&my_device, my_device_no, 1) == -1) {
        printk("Failed to register device to kernel\n");
        goto AddError;
    }
    return 0;

AddError:
        device_destroy(my_class, my_device_no);
FileError:
        class_destroy(my_class);
ClassError:
        unregister_chrdev(my_device_no, DRIVER_NAME);
        return -1;
}

static void __exit ModuleExit(void) {
    cdev_del(&my_device);
    device_destroy(my_class, my_device_no);
    class_destroy(my_class);
    unregister_chrdev(my_device_no, DRIVER_NAME);
    printk(KERN_INFO "Module unloaded\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eeshan");
MODULE_DESCRIPTION("Register a device number and implement a callback function");