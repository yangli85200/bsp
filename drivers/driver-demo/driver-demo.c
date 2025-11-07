#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "driver_demo"
#define CLASS_NAME "driver_demo_class"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pantum");
MODULE_DESCRIPTION("A simple demo driver for Pantum printer");
MODULE_VERSION("1.0");

static int major_number;
static struct class *demo_class = NULL;
static struct device *demo_device = NULL;

static int demo_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "driver-demo: Device opened\n");
    return 0;
}

static int demo_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "driver-demo: Device closed\n");
    return 0;
}

static ssize_t demo_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset)
{
    char message[] = "Hello from driver-demo!\n";
    size_t message_len = strlen(message);
    int error_count = 0;

    if (*offset >= message_len)
        return 0;

    if (len > message_len - *offset)
        len = message_len - *offset;

    error_count = copy_to_user(buffer, message + *offset, len);

    if (error_count == 0) {
        *offset += len;
        printk(KERN_INFO "driver-demo: Sent %zu characters to user\n", len);
        return len;
    } else {
        printk(KERN_INFO "driver-demo: Failed to send %d characters to user\n", error_count);
        return -EFAULT;
    }
}

static ssize_t demo_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset)
{
    printk(KERN_INFO "driver-demo: Received %zu characters from user\n", len);
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
    printk(KERN_INFO "driver-demo: Initializing the driver\n");

    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "driver-demo: Failed to register a major number\n");
        return major_number;
    }
    printk(KERN_INFO "driver-demo: Registered with major number %d\n", major_number);

    demo_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(demo_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "driver-demo: Failed to register device class\n");
        return PTR_ERR(demo_class);
    }
    printk(KERN_INFO "driver-demo: Device class registered\n");

    demo_device = device_create(demo_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(demo_device)) {
        class_destroy(demo_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "driver-demo: Failed to create the device\n");
        return PTR_ERR(demo_device);
    }
    printk(KERN_INFO "driver-demo: Device class created\n");

    return 0;
}

static void __exit demo_exit(void)
{
    device_destroy(demo_class, MKDEV(major_number, 0));
    class_unregister(demo_class);
    class_destroy(demo_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "driver-demo: Goodbye from the driver\n");
}

module_init(demo_init);
module_exit(demo_exit);

