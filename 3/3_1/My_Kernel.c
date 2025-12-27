#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <asm/current.h>
#include <linux/sched/prio.h>    // task_prio()
#include <linux/sched/signal.h>  // for_each_thread
#include <linux/uaccess.h>

#define procfs_name "Mythread_info"
#define BUFSIZE  1024
char buf[BUFSIZE];

static ssize_t Mywrite(struct file *fileptr, const char __user *ubuf, size_t buffer_len, loff_t *offset){
    /* Do nothing */
    return buffer_len;
}


static ssize_t Myread(struct file *fileptr, char __user *ubuf, size_t buffer_len, loff_t *offset){
    /*Your code here*/
    int len = 0;
    struct task_struct *t;

    if (*offset > 0)
        return 0;

    len += scnprintf(buf + len, BUFSIZE - len,
                     "Process ID(tgid): %d\n", current->tgid);

    /* current thread */
    len += scnprintf(buf + len, BUFSIZE - len,
                     "TID: %d  prio: %d  state: %u\n",
                     current->pid, current->prio, current->__state);

    /* other threads in same thread group (skip current to avoid duplicate) */
    for_each_thread(current, t) {
        if (t == current)
            continue;

        len += scnprintf(buf + len, BUFSIZE - len,
                         "TID: %d  prio: %d  state: %u\n",
                         t->pid, t->prio, t->__state);

        if (len >= BUFSIZE)
            break;
    }

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
    proc_create(procfs_name, 0644, NULL, &Myops);   
    pr_info("My kernel says Hi");
    return 0;
}

static void My_Kernel_Exit(void){
    pr_info("My kernel says GOODBYE");
}

module_init(My_Kernel_Init);
module_exit(My_Kernel_Exit);

MODULE_LICENSE("GPL");
