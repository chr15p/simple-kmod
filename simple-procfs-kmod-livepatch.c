// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * livepatch-sample.c - Kernel Live Patching Sample Module
 *
 * Copyright (C) 2014 Seth Jennings <sjenning@redhat.com>
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/livepatch.h>

#define BUFSIZE  100

#define PROCFS_NAME "simple_procfs_kmod"
static int number=0;

static ssize_t livepatch_myread(struct file *file, char __user *ubuf,size_t count, loff_t *ppos)
{
    char buf[BUFSIZE];
    int len=0;
    if(*ppos > 0 || count < BUFSIZE)
        return 0;
    len += sprintf(buf,"%s HAS BEEN PATCHED = %d\n",PROCFS_NAME, number);
    printk("%s HAS BEEN PATCHED = %d\n",PROCFS_NAME, number);
    if(raw_copy_to_user(ubuf,buf,len))
        return -EFAULT;
    *ppos = len;
    return len;
}


static struct klp_func funcs[] = {
        {
                .old_name = "myread",
                .new_func = livepatch_myread,
        }, { }
};

static struct klp_object objs[] = {
        {
                /* name being NULL means vmlinux */
                .name = PROCFS_NAME,
                .funcs = funcs,
        }, { }
};

static struct klp_patch patch = {
        .mod = THIS_MODULE,
        .objs = objs,
        .replace = true,
};

static int livepatch_init(void)
{
        return klp_enable_patch(&patch);
}

static void livepatch_exit(void)
{
}

module_init(livepatch_init);
module_exit(livepatch_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Chris Procter");
MODULE_INFO(livepatch, "Y");

