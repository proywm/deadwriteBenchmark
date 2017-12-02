

// syscall stealing example.
// http://www.tldp.org/LDP/lkmpg/2.6/html/lkmpg.html#AEN976

/* 
 * Copyright (C) 2001 by Peter Jay Salzman 
 */

/* 
 * The necessary header files 
 */

/*
 * Standard in kernel modules 
 */
#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/module.h>	/* Specifically, a module, */
#include <linux/moduleparam.h>	/* which will have params */
#include <linux/unistd.h>	/* The list of system calls */

/* 
 * For the current (process) structure, we need
 * this to know who the current user is. 
 */
#include <linux/sched.h>
#include <asm/uaccess.h>

#include <linux/init.h>      // included for __init and __exit macros
#include <linux/slab.h>


/*
dead_compute:

This is used to create deadwrite/killing writes
in two for loops. Similar to dead_compute01, but
with no read between two write loops.

- given an array int *a and its size, write
  new values to every a[i] in two for loops.
*/

int dead_compute( int *a, int size){

   int j=0 ;
   long count=0;

   for (j=0; j<size*3; j++){
   
	int i, tmp=0;
	
	// first loop to write a[i]s.
	for (i = 0; i<size; i++){
		a[i] = i*i-tmp;
	        tmp += i;
	}

	// second loop to write a[i]s
	for (i =0 ; i< size; i++){
		a[i] = i*i/2 + 1;
                tmp = tmp + 2*i -200;
	}

	count += tmp * 2 - tmp / 3;

  }
  return 0;
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("LELE MA");
MODULE_DESCRIPTION("A kernel module overwrites the syscall printk ");



/* 
 * The system call table (a table of functions). We
 * just define this as external, and the kernel will
 * fill it up for us when we are insmod'ed
 *
 * sys_call_table is no longer exported in 2.6.x kernels.
 * If you really want to try this DANGEROUS module you will
 * have to apply the supplied patch against your current kernel
 * and recompile it.
 */
extern void *sys_call_table[];

/* 
 * A pointer to the original system call. The reason
 * we keep this, rather than call the original function
 * (sys_open), is because somebody else might have
 * replaced the system call before us. Note that this
 * is not 100% safe, because if another module
 * replaced sys_open before us, then when we're inserted
 * we'll call the function in that module - and it
 * might be removed before we are.
 *
 * Another reason for this is that we can't get sys_open.
 * It's a static variable, so it is not exported. 
 */
asmlinkage int (*original_call) (const char *, int, int);

/* 
 * The function we'll replace sys_open (the function
 * called when you call the open system call) with. To
 * find the exact prototype, with the number and type
 * of arguments, we find the original function first
 * (it's at fs/open.c).
 *
 * In theory, this means that we're tied to the
 * current version of the kernel. In practice, the
 * system calls almost never change (it would wreck havoc
 * and require programs to be recompiled, since the system
 * calls are the interface between the kernel and the
 * processes).
 */
asmlinkage int our_sys_open(const char *filename, int flags, int mode)
{

	/* 
	 * do some dead writes.
	 */

	int size = 2000;
	int i;
	int *a;
	a = kmalloc (size * sizeof(int), GFP_KERNEL);

	printk(KERN_INFO "Starting syscall_array_test with deadwrites\n");

	dead_compute( a, size);

	for (i = size-1; i>=0; i--){
		printk(KERN_CONT "a[%d]=%d\t", i, a[i]);

		if(i%10==0) printk(KERN_CONT "\n");
	}

	kfree(a);


	/* 
	 * Call the original sys_open - otherwise, we lose
	 * the ability to open files 
	 */
	return original_call(filename, flags, mode);

}


static int __init syscall_array_test_init(void)
{
	
	printk(KERN_INFO "Initiating syscall_array_test with deadwrites\n");

    /* 
	 * Warning - too late for it now, but maybe for
	 * next time... 
	 */
	printk(KERN_ALERT "I'm dangerous. I hope you did a ");
	printk(KERN_ALERT "sync before you insmod'ed me.\n");
	printk(KERN_ALERT "My counterpart, cleanup_module(), is even");
	printk(KERN_ALERT "more dangerous. If\n");
	printk(KERN_ALERT "you value your file system, it will ");
	printk(KERN_ALERT "be \"sync; rmmod\" \n");
	printk(KERN_ALERT "when you remove this module.\n");

	/* 
	 * Keep a pointer to the original function in
	 * original_call, and then replace the system call
	 * in the system call table with our_sys_open 
	 */
	original_call = sys_call_table[__NR_open];
	sys_call_table[__NR_open] = our_sys_open;

	/* 
	 * To get the address of the function for system
	 * call foo, go to sys_call_table[__NR_foo]. 
	 */

	printk(KERN_INFO "Now you can call open with deadwrites\n");

	return 0;
}


/* 
 * Cleanup - unregister the appropriate file from /proc 
 */

static void __exit syscall_array_test_cleanup(void)
{
	/* 
	 * Return the system call back to normal 
	 */
	if (sys_call_table[__NR_open] != our_sys_open) {
		printk(KERN_ALERT "Somebody else also played with the ");
		printk(KERN_ALERT "open system call\n");
		printk(KERN_ALERT "The system may be left in ");
		printk(KERN_ALERT "an unstable state.\n");
	}

	sys_call_table[__NR_open] = original_call;

    printk(KERN_INFO "Goodbye from syscall_array_test.\n");
}

module_init(syscall_array_test_init);
module_exit(syscall_array_test_cleanup);

