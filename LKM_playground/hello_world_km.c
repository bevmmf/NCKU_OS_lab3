// header
#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>

// code
static int __init kernel_init(void)
{
	pr_info("hello world!!\n");
	return 0;
}

static void __exit kernel_exit(void)
{
	pr_info("goodbye world!!\n");
}
// registration
module_init(kernel_init);
module_exit(kernel_exit);
// metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("bev");
MODULE_DESCRIPTION("Hello world(my first time in LKM)!!!!");
MODULE_INFO(host,"bev_12F_VM(OS_LAB)");

