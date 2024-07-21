#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define MY_MAJOR 95
#define MY_MINOR 0 /* NOT USED??? */
#define DRIVER_NAME "dummy_driver"
#define DRIVER_CLASS "module_class"

MODULE_LICENSE("GPL");

static char* buffer;
static size_t buffer_head = 0, buffer_tail = 0;
static unsigned int buffer_size;

static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

module_param(buffer_size, uint, S_IRUGO);
MODULE_PARM_DESC(buffer_size, "Size of the ring buffer");

static int driver_open(struct inode* device_file, struct file* instance) {
  printk("open called!\n");
  return 0;
}

static int driver_close(struct inode* device_file, struct file* instance) {
  printk("close called!\n");
  return 0;
}
/*
static ssize_t driver_read(struct file* filp, char* user_buffer, size_t count, loff_t* offs) {
  int to_copy, not_copied, remaining_space, delta;
  
  to_copy = min(count - 1, buffer_size);
  remaining_space = buffer_size - buffer_head;
  not_copied = copy_to_user(user_buffer, buffer + buffer_head, remaining_space);
  not_copied += copy_to_user(user_buffer + remaining_space, buffer, to_copy - remaining_space);
  if ((buffer_head += remaining_space) >= buffer_size) {
    buffer_head -= buffer_size;
  }
  delta = to_copy - not_copied;
  printk("Delta: %d\n", delta);
  return delta;
}

static ssize_t driver_write(struct file *filp, const char *user_buffer, size_t count, loff_t *offs) {
  int to_copy, not_copied, remaining_space, delta;

  if (count > buffer_size + 1) {
    printk("Size limit exceeded! Count: %d, Size: %d\n", count, buffer_size);
    return -1;
  }

  to_copy = count - 1;
  remaining_space = buffer_size - buffer_tail;
  not_copied = copy_from_user(buffer + buffer_tail, user_buffer, remaining_space);
  not_copied += copy_from_user(buffer, user_buffer + remaining_space, to_copy - remaining_space);
  if ((buffer_tail += remaining_space) >= buffer_size) {
    buffer_tail -= buffer_size;
  }
  delta = to_copy - not_copied;
  printk("Delta: %d\n", delta);
  return delta;
}
*/

static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;
	to_copy = min(count, buffer_size);
	not_copied = copy_to_user(user_buffer, buffer, to_copy);
	delta = to_copy - not_copied;
    printk("Reading called! Delta: %d\n");

	return delta;
}

static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;
	to_copy = min(count, buffer_size);
	not_copied = copy_from_user(buffer, user_buffer, to_copy);
	delta = to_copy - not_copied;
    printk("Writing called! Delta: %d\n");

	return delta;
}

static struct file_operations fops = {
  .owner = THIS_MODULE,
  .open = driver_open,
  .release = driver_close,
  .read = driver_read,
  .write = driver_write
};

static int __init ModuleInit(void) {
  int retval;
  printk("Hello, Kernel!\n");
  buffer = kmalloc(buffer_size, GFP_KERNEL);
  if( alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
    printk("Device Nr. could not be allocated!\n");
    return -1;
  }
  printk("read_write - Device Nr. Major: %d, Minor: %d was registered!\n", my_device_nr >> 20, my_device_nr & 0xfffff);

  if((my_class = class_create(DRIVER_CLASS)) == NULL) {
    printk("Device class can not be created!\n");
    goto ClassError;
  }

  if(device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
    printk("Can not create device file!\n");
    goto FileError;
  }

  cdev_init(&my_device, &fops);

  if(cdev_add(&my_device, my_device_nr, 1) == -1) {
    printk("Registering of device to kernel failed!\n");
    goto AddError;
  }

  return 0;
AddError:
  device_destroy(my_class, my_device_nr);
FileError:
  class_destroy(my_class);
ClassError:
  unregister_chrdev_region(my_device_nr, 1);
  return -1;
}

static void __exit ModuleExit(void) {
  cdev_del(&my_device);
  device_destroy(my_class, my_device_nr);
  class_destroy(my_class);
  unregister_chrdev_region(my_device_nr, 1);
  printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

