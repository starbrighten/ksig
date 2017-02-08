/*
 * signal_kernel.c
 * A kernel module used to send signals to the user space
 * Originated from http://people.ee.ethz.ch/~arkeller/linux/code/signal_kernel_phil_bergeron_burns.c
 * Modified by daveti
 * Aug 23, 2014
 * root@davejingtian.org
 * http://davejingtian.org
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/siginfo.h>	00//siginfo
#include <linux/rcupdate.h>	//rcu_read_lock
#include <linux/sched.h>	//find_task_by_pid_type
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/random.h>


#define SIG_TEST 44	// we choose 44 as our signal number (real-time signals are in the range of 33 to 64)

static char *name = "PracticeTemp";
module_param(name, charp, S_IRUGO);
static struct file_operations fops;

static char msg[128];
static int len = 0;
static int len_check = 1;
static unsigned int temp = 100;
//struct dentry *file;

/*static ssize_t open_pid(struct inode *sp_inode, struct file *file){
    pr_info("open_pid");
    return 0;
}

static ssize_t release_pid(struct inode *sp_inode, struct file *file){
    pr_info("release_pid");
    return 0;
}

static char mybuf[10];
static int len = 0;
static int len_check = 1;

static ssize_t read_pid(struct file *file, char __user *buf, 
								size_t count, loff_t *offset)
{

    if (len_check){
	len_check = 0;
    }else {
	len_check = 1;
	return 0;
    }
    pr_info("read_pid");
    copy_to_user(buf,mybuf,len);
    return len;
}*/

static ssize_t write_pid(struct file *file, const char __user *buf,
                                size_t count, loff_t *ppos)
{
	char mybuf[10];
	int pid = 0;
	int ret;
	struct siginfo info;
	struct task_struct *t;
	/* read the value from user space */
	if(count > 10)
		return -EINVAL;
	len = count;
	copy_from_user(mybuf, buf, count);
	sscanf(mybuf, "%d", &pid);
	pr_info("pid = %d\n", pid);

	/* prepare the signal */
	memset(&info, 0, sizeof(struct siginfo));
	info.si_signo = SIG_TEST;
	/*
	this is bit of a trickery: SI_QUEUE is normally used by sigqueue from user space,
	and kernel space should use SI_KERNEL. But if SI_KERNEL is used the real_time data 
	is not delivered to the user space signal handler function. 
	*/
	info.si_code = SI_QUEUE;
	/* real time signals may have 32 bits of data. */
	info.si_int = 1234;

	rcu_read_lock();
	/* find the task with that pid */
	t = pid_task(find_pid_ns(pid, &init_pid_ns), PIDTYPE_PID);	
	if(t == NULL){
		pr_err("no such pid\n");
		rcu_read_unlock();
		return -ENODEV;
	}
	rcu_read_unlock();

	if(temp != 100){
		/* send the signal */
		ret = send_sig_info(SIG_TEST, &info, t);    //send the signal
		if (ret < 0) {
			pr_err("error sending signal\n");
			return ret;
		}		
	}

	return count;
}

static const struct file_operations my_fops = {
	//.open = open_pid,
	//.read = read_pid,
	.write = write_pid,
	//.release = release_pid
};

int open_practicetemp(struct inode *sp_inode, struct file *sp_file){
    printk(KERN_INFO "PracticeTemp_proc_OPEN!!");
    return 0;
}

int release_practicetemp(struct inode *sp_inode, struct file *sp_file){
    printk(KERN_INFO "PracticeTemp_proc_Release!!");
    return 0;
}

int read_practicetemp(struct file *sp_file, char __user *buf, size_t size, loff_t *offset){

    if (len_check){
	len_check = 0;
    }else {
	len_check = 1;
	return 0;
    }
    printk(KERN_INFO "PracticeTemp_proc_Read!!");
    copy_to_user(buf,msg,len);
    return len;
}

int write_practicetemp(struct file *sp_file, const char __user *buf, size_t size, loff_t *offset){

    printk(KERN_INFO "PracticeTemp_proc_Write!!");
    len = size;
    copy_from_user(msg,buf,len);
    unsigned int randNum;
    get_random_bytes(&randNum, sizeof(unsigned int));
    randNum = randNum%100;
    temp = randNum;
    printk(KERN_INFO "We get random number: %d \n", temp);
    return size;
}

static struct file_operations fops = {
	.open = open_practicetemp,
	.read = read_practicetemp,
	.write = write_practicetemp,
	.release = release_practicetemp
};



static int __init ksig_init(void)
{
	pr_info("into %s\n", __func__);
	/* we need to know the pid of the user space process
 	 * -> we use debugfs for this. As soon as a pid is written to 
 	 * this file, a signal is sent to that pid
 	 */
	/* only root can write to this file (no read) */
	//debugfs_create_file("signalconfpid", 0200, NULL, NULL, &my_fops);
	if(! proc_create("signalconfpid",0666,NULL,&my_fops)){
	printk(KERN_INFO "Error! proc_create");
	remove_proc_entry("signalconfpid",NULL);
	return -1;
    }

    printk(KERN_INFO "Hello %s!\n",name);
    if(! proc_create("PracticeTemp",0666,NULL,&fops)){
	printk(KERN_INFO "Error! proc_create");
	remove_proc_entry("PracticeTemp",NULL);
	return -1;
    }

	return 0;
}
static void __exit ksig_exit(void)
{
	pr_info("exit ksig module\n");
	//debugfs_remove(file);
	remove_proc_entry("signalconfpid", NULL);
	remove_proc_entry("PracticeTemp",NULL);
}

module_init(ksig_init);
module_exit(ksig_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("ksig module");
MODULE_AUTHOR("daveti");
