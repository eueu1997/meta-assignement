#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include "data.h"
#include <linux/string.h>

static dev_t hrm_dev;

struct cdev hrm_cdev;

struct class *myclass = NULL;

static char buffer[64];

static int cnt=0;


ssize_t hrm_read(struct file *filp, char  *buf, size_t count, loff_t *f_pos)
{
    
    printk(KERN_INFO "reading value %d at pos %d \n",ppg[cnt], cnt );
    sprintf(buf,"%d",ppg[cnt]);
    cnt++;
    return count;
}

static int hrm_open(struct inode *inode, struct file *file)
{
cnt=0;
return 0;
}

struct file_operations hrm_fops = {
    .owner = THIS_MODULE,
    .read = hrm_read,
    .open = hrm_open,
};

static int __init hrm_module_init(void)
{
    printk(KERN_INFO "Loading hrm device\n");

    alloc_chrdev_region(&hrm_dev, 0, 1, "hrm_dev");
    printk(KERN_INFO "%s\n", format_dev_t(buffer, hrm_dev));

    myclass = class_create(THIS_MODULE, "hrm_sys");
    device_create(myclass, NULL, hrm_dev, NULL, "hrm_dev");

    cdev_init(&hrm_cdev, &hrm_fops);
    hrm_cdev.owner = THIS_MODULE;
    cdev_add(&hrm_cdev, hrm_dev, 1);

    return 0;
}

static void __exit hrm_module_cleanup(void)
{
    printk(KERN_INFO "Cleaning-up hrm_dev.\n");

    device_destroy(myclass, hrm_dev );
    cdev_del(&hrm_cdev);
    class_destroy(myclass);
    unregister_chrdev_region(hrm_dev, 1);
}

module_init(hrm_module_init);
module_exit(hrm_module_cleanup);
MODULE_AUTHOR("Eugenio Ressa");
MODULE_LICENSE("GPL");
