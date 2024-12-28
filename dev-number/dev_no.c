#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

#define MAJOR_NO 100

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
    .release = driver_close
};


static int __init ModuleInit(void) {
    int retval;
    printk(KERN_INFO "Module loaded\n");
    retval = register_chrdev(MAJOR_NO, "dev_number", &fops);
    if(retval == 0) {
        printk("dev_no - registered Device number Major: %d, Minor: %d\n", MAJOR_NO, 0);
    }
    else if(retval > 0) {
        printk("dev_no - registered Device number Major: %d, Minor: %d\n", retval>>20, retval&0xfffff);
    }
    else {
        printk("Could not register device number\n");
        return -1;
    }
    return 0;
}

static void __exit ModuleExit(void) {
    unregister_chrdev(MAJOR_NO, "dev_number");
    printk(KERN_INFO "Module unloaded\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eeshan");
MODULE_DESCRIPTION("Register a device number and implement a callback function");