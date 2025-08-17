#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AkaruiNeko");
MODULE_DESCRIPTION("ArcatLinux kernel module");

static int __init arcat_init(void) {
    printk(KERN_INFO "Arcat kernel loaded!\n");
    return 0;
}

module_init(arcat_init);
