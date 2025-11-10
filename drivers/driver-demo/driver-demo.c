#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>

#define DEVICE_NAME "driver_demo"
#define CLASS_NAME "demo"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pantum");
MODULE_DESCRIPTION("A simple demo driver");
MODULE_VERSION("1.0");

static int major_number;
static struct class *demo_class = NULL;
static struct device *demo_device = NULL;

static int demo_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "driver-demo: opened\n");
    return 0;
}

static int demo_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "driver-demo: closed\n");
    return 0;
}

static ssize_t demo_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset)
{
    printk(KERN_INFO "driver-demo: read\n");
    return 0;
}

static ssize_t demo_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset)
{
    printk(KERN_INFO "driver-demo: write\n");
    return len;
}

static struct file_operations fops = {
    .open = demo_open,
    .read = demo_read,
    .write = demo_write,
    .release = demo_release,
};

static int __init demo_init(void)
{
    printk(KERN_INFO "driver-demo: init\n");

    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "driver-demo: failed to register\n");
        return major_number;
    }

    demo_class = class_create(CLASS_NAME);
    if (IS_ERR(demo_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        return PTR_ERR(demo_class);
    }

    demo_device = device_create(demo_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(demo_device)) {
        class_destroy(demo_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        return PTR_ERR(demo_device);
    }

    return 0;
}

static void __exit demo_exit(void)
{
    device_destroy(demo_class, MKDEV(major_number, 0));
    class_destroy(demo_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "driver-demo: exit\n");
}

module_init(demo_init);
module_exit(demo_exit);

