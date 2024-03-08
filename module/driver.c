//driver.c

#include "numbaseball.h"

static int already_open = 0;
static int major;
static struct class *cls;

static int __init device_init(void);
static void __exit device_exit(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static int device_write(struct file *, const char __user *, size_t, loff_t *);

static struct file_operations device_fops = {
    .open = device_open,
    .release = device_release,
    .write = device_write,
};


// when insmod
static int __init device_init(void) {
    // register driver
    major = register_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME, &device_fops);

    if (major < 0) {
        printk("Registering char device failed with %d\n", major);
        return major;
    }

    printk("Timer device major number : %d\n", DEVICE_MAJOR_NUMBER);

    //create device file
    cls = class_create(THIS_MODULE, DEVICE_NAME);
    device_create(cls, NULL, MKDEV(DEVICE_MAJOR_NUMBER, 0), NULL, DEVICE_NAME);

    printk("Timer device file created : /dev/%s\n", DEVICE_NAME);

    fpga_iomap_devices();

    return SUCCESS;
}

// when rmmod
static void __exit device_exit(void) {
    //remove device file
    device_destroy(cls, MKDEV(DEVICE_MAJOR_NUMBER, 0));
    class_destroy(cls);

    //unregister driver
    unregister_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME);

    fpga_iounmap_devices();
}

//when open(), check if device is already opened
static int device_open(struct inode* inode, struct file* file) {
    if (already_open != 0) {
        return -EBUSY;
    }
    already_open = 1;

    inter_open();

    return SUCCESS;
}

//when close()
static int device_release(struct inode* inode, struct file* file) {
    already_open = 0;

    inter_release();

    return SUCCESS;
}

// when write()
static int device_write
(struct file *file, const char __user *buf, size_t count, loff_t *f_pos) {

    startGame();

    return 0;
}

module_init(device_init);
module_exit(device_exit);

MODULE_LICENSE("GPL");