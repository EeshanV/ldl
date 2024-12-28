#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>


static char *kmalloc_buf = NULL;
static char *vmalloc_buf = NULL;


static int __init mem_alloc_init(void) {
	size_t kmalloc_size = 128; // 128 bytes
	size_t vmalloc_size = 1024 * 1024; //1 MB

	pr_info("Memory Allocator Module loaded");

	kmalloc_buf = kmalloc(kmalloc_size, GFP_KERNEL);
	if(!kmalloc_buf) {
		pr_err("kmalloc failed\n");
		return -ENOMEM;
	}
	

	snprintf(kmalloc_buf, kmalloc_size, "Hello - kmalloc");
	pr_info("kmalloc buffer address: %p, content: %s\n", kmalloc_buf, kmalloc_buf);
	
	vmalloc_buf = vmalloc(vmalloc_size);
	if(!vmalloc_buf) {
		pr_err("vmalloc failed\n");
		kfree(kmalloc_buf);
		return -ENOMEM;
	}
	
	snprintf(vmalloc_buf, vmalloc_size, "Hello - vmalloc");
	pr_info("vmalloc buffer address: %p, content: %s\n", vmalloc_buf, vmalloc_buf);

	return 0;
}

static void __exit mem_alloc_exit(void) {
	pr_info("Memory Allocator Module Unloaded\n");

	if(kmalloc_buf) {
		kfree(kmalloc_buf);
		pr_info("kmalloc buffer freed\n");
	}

	if(vmalloc_buf) {
		vfree(vmalloc_buf);
		pr_info("vmalloc buffer freed\n");
	}
}

module_init(mem_alloc_init);
module_exit(mem_alloc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eeshan");
MODULE_DESCRIPTION("Memory Allocator example");
