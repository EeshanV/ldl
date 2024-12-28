#include <linux/module.h>
#include <linux/init.h>

static int __init ModuleInit(void) {
    printk(KERN_INFO "Module loaded\n");
    return 0;
}

static void __exit ModuleExit(void) {
    printk(KERN_INFO "Module unloaded\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eeshan");
