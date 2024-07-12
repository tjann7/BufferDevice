#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

#define MY_MAJOR 95
#define MY_MINOR 0 /* NOT USED??? */

MODULE_LICENSE("GPL");

static int driver_open(struct inode* device_file, struct file* instance) {
	printk("open called!\n");
	return 0;
}

static int driver_close(struct inode* device_file, struct file* instance) {
	printk("close called!\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close
};



static int __init ModuleInit(void) {
	printk("Initialized Module first symdriver\n");
	int ret = register_chrdev(MY_MAJOR, "first symdriver", &fops);
	if (ret == 0) {
		printk("first symdriver registered! Major: %d, Minor: %d\n", MY_MAJOR, 0);
	} else if (ret > 0) {
		printk("first symdriver registered! Major: %d, Minor: %d\n", ret>>20, ret&0xfffff);
	} else {
		printk("First symdriver unregistered! :(\n");
		return -1;
	}
	return 0;
}

static void __exit ModuleExit(void) {
	unregister_chrdev(MY_MAJOR, "first symdriver");
	printk("Exit first symdriver\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

