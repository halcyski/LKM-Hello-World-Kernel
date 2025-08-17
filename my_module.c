#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");

static int __init hello_init(void) {
	printk(KERN_INFO "LKM: Hello kernel!");
	return 0;
}

static void __exit hello_exit(void) {
	printk(KERN_INFO "LKM: Goodbye kernel!");
}

module_init(hello_init);
module_exit(hello_exit);
