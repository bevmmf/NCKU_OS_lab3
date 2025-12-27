#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <asm/current.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <linux/string.h>

#define procfs_name "Mythread_info"
#define BUFSIZE  1024
char buf[BUFSIZE]; //kernel buffer
#define MAX_MSG 128
#define MAX_ENT 64
static struct proc_dir_entry *my_entry;

struct ent {
    pid_t pid;
    char  msg[MAX_MSG];
};

static struct ent table[MAX_ENT];
static DEFINE_SPINLOCK(table_lock);

static ssize_t Mywrite(struct file *fileptr, const char __user *ubuf, size_t buffer_len, loff_t *offset){
    /*Your code here*/
    char kbuf[MAX_MSG];
    size_t n = buffer_len;
    int i, free_i = -1;

    if (n >= MAX_MSG)
        n = MAX_MSG - 1;

    if (copy_from_user(kbuf, ubuf, n))
        return -EFAULT;

    kbuf[n] = '\0';

    spin_lock(&table_lock);

    /* Search for a matching PID to update; otherwise, find a free slot. */
    for (i = 0; i < MAX_ENT; i++) {
        if (table[i].pid == current->pid) {
            strscpy(table[i].msg, kbuf, MAX_MSG);
            spin_unlock(&table_lock);
            return buffer_len;
        }
        if (free_i < 0 && table[i].pid == 0)
            free_i = i;
    }

    if (free_i < 0)
        free_i = 0; // If full, overwrite index 0 (sufficient for this lab). 

    table[free_i].pid = current->pid;
    strscpy(table[free_i].msg, kbuf, MAX_MSG);

    spin_unlock(&table_lock);
    return buffer_len;
    /****************/
}


static ssize_t Myread(struct file *fileptr, char __user *ubuf, size_t buffer_len, loff_t *offset){
    /*Your code here*/
    int len = 0;
    int i;
    char mymsg[MAX_MSG] = "(no message)";
    unsigned long flags;

    if (*offset > 0)
        return 0;

    spin_lock_irqsave(&table_lock, flags);
    for (i = 0; i < MAX_ENT; i++) {
        if (table[i].pid == current->pid) {
            strscpy(mymsg, table[i].msg, MAX_MSG);
            break;
        }
    }
    spin_unlock_irqrestore(&table_lock, flags);

    len = scnprintf(buf, BUFSIZE,
                    "String: %s\n"
                    "Process ID(tgid): %d\n"
                    "Thread  ID(pid) : %d\n"
                    "Time(ms)        : %llu\n",
                    mymsg,
                    current->tgid,
                    current->pid,
                    (unsigned long long)(current->utime / 100 / 1000));

    if (len > buffer_len)
        len = buffer_len;

    if (copy_to_user(ubuf, buf, len))
        return -EFAULT;

    *offset = len;
    return len;
    /****************/
}

static struct proc_ops Myops = {
    .proc_read = Myread,
    .proc_write = Mywrite,
};

static int My_Kernel_Init(void){
    remove_proc_entry(procfs_name, NULL); // prevent already registered
    my_entry = proc_create(procfs_name, 0644, NULL, &Myops);   
    if(!my_entry) return -ENOMEM;
    pr_info("My kernel says Hi");
    return 0;
}

static void My_Kernel_Exit(void){
    if (my_entry) proc_remove(my_entry);
    else remove_proc_entry(procfs_name, NULL);
    pr_info("My kernel says GOODBYE");
}

module_init(My_Kernel_Init);
module_exit(My_Kernel_Exit);

MODULE_LICENSE("GPL");
