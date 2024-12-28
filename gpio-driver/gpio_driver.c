#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

static dev_t my_device_no;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "my_gpio_driver"
#define DRIVER_CLASS "MyModuleClass"

static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
    int to_copy, not_copied, delta;
    char tmp[3] = " \n";

    to_copy = min(count, sizeof(tmp));

    printk("value of button: %d", gpio_get_value(17));
    tmp[0] = gpio_get_value(17) + '0';

    not_copied = copy_to_user(user_buffer, &tmp, to_copy);

    delta = to_copy - not_copied;
    return delta;
}

static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
    int to_copy, not_copied, delta;
    char value;

    to_copy = min(count, sizeof(value));

    not_copied = copy_from_user(&value, user_buffer, to_copy);

    switch(value) {
            case '0':
                gpio_set_value(4, 0);
                break;
            case '1':
                gpio_set_value(4, 1);
                break;
            default:
                printk("Invalid input");
                break;
    }

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
    
    if( alloc_chrdev_region(&my_device_no, 0, 1, DRIVER_NAME) < 0) {
        printk("Device number could not be allocated");
        return -1;
    }
    printk("read_write - Device number Major: %d, Minor: %d", my_device_no >> 20, my_device_no && 0xfffff);

	if((my_class = class_create(DRIVER_CLASS)) == NULL) {
        printk("Device class cannot be created");
        goto ClassError;
    }
    
    if(device_create(my_class, NULL, my_device_no, NULL, DRIVER_NAME) == NULL) {
        printk("Cannot create device file");
        goto FileError;
    }

    cdev_init(&my_device, &fops);

    if(cdev_add(&my_device, my_device_no, 1) == -1) {
        printk("Failed to register device to kernel");
        goto AddError;
    }

    /*GPIO 4 init*/
    if(gpio_request(4, "rpi-gpio-4")) {
        printk("Cannot allocate GPIO 4");
        goto AddError;
    }

    if(gpio_direction_output(4, 0)) {
        printk("Cannot set GPIO 4 to output");
        goto Gpio4Error;
    }    

    /*GPIO 17 init*/
    if(gpio_request(17, "rpi-gpio-17")) {
        printk("Cannot allocate GPIO 17");
        goto Gpio4Error;
    }

    if(gpio_direction_input(17)) {
        printk("Cannot set GPIO 17 to input");
        goto Gpio17Error;
    }  

    return 0;

Gpio17Error:
        gpio_free(17);
Gpio4Error:
        gpio_free(4);
AddError:
        device_destroy(my_class, my_device_no);
FileError:
        class_destroy(my_class);
ClassError:
        unregister_chrdev(my_device_no, DRIVER_NAME);
        return -1;
}

static void __exit ModuleExit(void) {
    gpio_set_value(4, 0);
    gpio_free(17);
    gpio_free(4);
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
MODULE_DESCRIPTION("GPIO driver for a simple LED and button control");