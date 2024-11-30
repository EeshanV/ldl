#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/semaphore.h>

#define DEVICE_NAME "semaphore_dev"
#define BUFFER_SIZE 256

static int major_number;
static char device_buffer[BUFFER_SIZE];
static int data_size = 0;
static int is_buffer_empty = 1;

static struct semaphore sem;
static DECLARE_WAIT_QUEUE_HEAD(wait_queue);

static int dev_open(struct inode *inode, struct file *filep);
static int dev_release(struct inode *inode, struct file *filep);
static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset);
static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset);


static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};


static int __init semaphore_init(void){
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0){
        printk(KERN_ALERT "Failed to register device\n");
        return major_number;
    }

    sema_init(&sem, 1);
    printk(KERN_INFO "Semaphore Driver registered with major number %d\n", major_number);
    return 0;
}

static void __exit semaphore_exit(void){
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Semaphore Driver unregistered\n");
}

static int dev_open(struct inode *inode, struct file *filep){
    printk(KERN_INFO "Device Opened\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *filep){
    printk(KERN_INFO "Device Closed\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    ssize_t bytes_to_copy;
    
    if(wait_event_interruptible(wait_queue, !is_buffer_empty)) {
        return -ERESTARTSYS;
    }

    if(down_interruptible(&sem)) {
        return -ERESTARTSYS;
    }

    bytes_to_copy = min(len, (size_t)data_size);
    if (copy_to_user(buffer, device_buffer, bytes_to_copy)) {
        up(&sem);
        return -EFAULT;
    }

    is_buffer_empty = 1;

    up(&sem);
    printk(KERN_INFO "Device read: %ld bytes\n", bytes_to_copy);
    return bytes_to_copy;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    ssize_t bytes_to_copy;

    if(down_interruptible(&sem)) {
        return -ERESTARTSYS;
    }

    if(!is_buffer_empty){
        up(&sem);
        return -EBUSY;
    }

    bytes_to_copy = min(len, (size_t)(BUFFER_SIZE - data_size));
    if (copy_from_user(device_buffer, buffer, bytes_to_copy)) {
        up(&sem);
        return -EFAULT;
    }

    data_size = bytes_to_copy;
    is_buffer_empty = 0;

    up(&sem);
    printk(KERN_INFO "Device written: %ld bytes\n", bytes_to_copy);
    return bytes_to_copy;
}

module_init(semaphore_init);
module_exit(semaphore_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eeshan");
MODULE_DESCRIPTION("Character driver using semaphores to manage concurrent access");