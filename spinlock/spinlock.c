#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>

#define DEVICE_NAME "spinlock_device"
#define BUFFER_SIZE 256

static int major_number;
static char device_buffer[BUFFER_SIZE];
static int data_size = 0;
static int open_count=0;

static spinlock_t buffer_lock;
static spinlock_t counter_lock;

static int dev_open(struct inode *inodep, struct file *filep);
static int dev_release(struct inode *inodep, struct file *filep);
static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset);
static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset);

static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};

static int __init spinlock_driver_init(void) {
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "Failed to register device\n");
        return major_number;
    }

    spin_lock_init(&buffer_lock);
    spin_lock_init(&counter_lock);

    printk(KERN_INFO "Spinlock Driver loaded with major number %d\n", major_number);
    return 0;
}

static void __exit spinlock_driver_exit(void) {
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Spinlock Driver unloaded\n");
}

static int dev_open(struct inode *inode, struct file *file) {
    unsigned long flags;

    spin_lock_irqsave(&counter_lock, flags);
    open_count++;
    spin_unlock_irqrestore(&counter_lock, flags);

    printk(KERN_INFO "Device Opened %d times\n", open_count);
    return 0;
}

static int dev_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device CLosed\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    unsigned long flags;
    ssize_t bytes_to_copy;

    spin_lock_irqsave(&buffer_lock, flags);

    if (data_size == 0) {
        spin_unlock_irqrestore(&buffer_lock, flags);
        printk(KERN_INFO "Device read: No data available\n");
        return 0;
    }

    bytes_to_copy = min(len, (size_t)data_size);
    if (copy_to_user(buffer, device_buffer, bytes_to_copy)) {
    spin_unlock_irqrestore(&buffer_lock, flags);
    return -EFAULT;
}

    
    if (bytes_to_copy == data_size) {
        data_size = 0;
    }

    spin_unlock_irqrestore(&buffer_lock, flags);

    printk(KERN_INFO "Device read: %ld bytes\n", bytes_to_copy);
    return bytes_to_copy;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    unsigned long flags;
    ssize_t bytes_to_copy;

    spin_lock_irqsave(&buffer_lock, flags);

    if (data_size >= BUFFER_SIZE) {
        spin_unlock_irqrestore(&buffer_lock, flags);
        return -ENOMEM;
    }

    bytes_to_copy = min(len, (size_t)(BUFFER_SIZE - data_size));
    if (copy_from_user(device_buffer + data_size, buffer, bytes_to_copy)) {
        spin_unlock_irqrestore(&buffer_lock, flags);
        return -EFAULT;
    }
    data_size += bytes_to_copy;

    spin_unlock_irqrestore(&buffer_lock, flags);

    printk(KERN_INFO "Device written: %ld bytes\n", bytes_to_copy);
    return bytes_to_copy;
}

module_init(spinlock_driver_init);
module_exit(spinlock_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eeshan");
MODULE_DESCRIPTION("Character driver using semaphores to manage concurrent access");