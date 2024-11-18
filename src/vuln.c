#include "linux/printk.h"
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

MODULE_AUTHOR("Qanux");
MODULE_LICENSE("Dual BSD/GPL");

char note = NULL;

static long vuln_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    unsigned long note_size = arg;

    if(cmd == 0x6666){
        if(note_size > 0x2000){
            return -1;
        }

        note = kmalloc(note_size, GFP_KERNEL);
        if(note == NULL){
            return -1;
        }
    }else if(cmd == 0x7777){
        kfree(note);
    }else{
        return -1;
    }

    return 0;
}

static ssize_t vuln_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos){
    if(count > 0x2000){
        return -1;
    }

    if (copy_from_user(note, buf, count)) {
        return -1;
    }

    return count;
}

static ssize_t vuln_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos){
    if(count > 0x2000){
        return -1;
    }

    if (copy_to_user(buf, note, count)) {
        return -1;
    }

    return count;
}

static struct file_operations vuln_fops = {.owner = THIS_MODULE,
                                           .open = NULL,
                                           .release = NULL,
                                           .read = vuln_read,
                                           .write = vuln_write,
                                           .unlocked_ioctl = vuln_ioctl};

static struct miscdevice vuln_miscdev = {
    .minor = MISC_DYNAMIC_MINOR, .name = "vuln", .fops = &vuln_fops};

static int __init vuln_init(void) {
    pr_info("vuln: module init.\n");
    misc_register(&vuln_miscdev);
    return 0;
}

static void __exit vuln_exit(void) {
    pr_info("vuln: module exit.\n");
    misc_deregister(&vuln_miscdev);
}

module_init(vuln_init);
module_exit(vuln_exit);
