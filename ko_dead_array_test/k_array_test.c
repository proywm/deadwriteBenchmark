#include <linux/module.h> // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros
#include <linux/slab.h>

//#include <stdio.h>
//#include <stdlib.h>

/*
dead_compute01:

This is used to create deadwrite/killing writes

in two for loops.

- given an array int *a and its size, write

  new values to every a[i] in two for loops.

  Between two for loops, do some reading in a.

- int *a
- int size

*/

int dead_compute01( int *a, int size){

   int j=0 ;
   long count=0;

   for (j=0; j<size*3; j++){

	int i, tmp=0;

	// first loop to write a[i]s.
	for (i = 0; i<size; i++){
		a[i] = i*i-tmp;
	        tmp += i;
	}

	// do some reading
	if(j%10 == 0) printk(KERN_INFO "%s:", __FUNCTION__);
	printk(KERN_CONT "(j=%d)\t%d",j, a[(i+j)/4]);
	if(j%10 == 0) printk(KERN_INFO "\n");

	// second loop to write a[i]s
	for (i =0 ; i< size; i++){
		a[i] = i*i/2 - 1;
                tmp = tmp + 2*i -200;
	}

	count += tmp * 2 - tmp / 3;

    }
	return 0;
}

/*
dead_compute02:

This is used to create deadwrite/killing writes

in two for loops. Similar to dead_compute01, but

with no read between two write loops.

- given an array int *a and its size, write

  new values to every a[i] in two for loops.

- int *a
- int size

*/

int dead_compute02( int *a, int size){

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
MODULE_DESCRIPTION("A sample kernel module with simple deadwrites");

static int __init k_array_test_init(void)
{
	int size = 2000;
	int i;
	int *a, *b;

	a = kmalloc (size * sizeof(int), GFP_KERNEL);
	b = kmalloc (size * sizeof(int), GFP_KERNEL);

	printk(KERN_INFO "Starting k_array_test with deadwrites\n");

	dead_compute01( a, size);

	dead_compute02( b, size);

	for (i = size-1; i>=0; i--){
		printk(KERN_CONT "a[%d]=%d\t", i, a[i]);
		printk(KERN_CONT "b[%d]=%d\t", i, b[i]);

		if(i%10==0) printk(KERN_CONT "\n");
	}

	kfree(a);
	kfree(b);
    return 0;    // Non-zero return means that the module couldn't be loaded.
}

static void __exit k_array_test_cleanup(void)
{
    printk(KERN_INFO "Goodbye from k_array_test.\n");
}

module_init(k_array_test_init);
module_exit(k_array_test_cleanup);

//int init_module(void){
//	printk(KERN_INFO "HELLO WORLD!\n");
//
//	return 0;
//}

//void cleanup_module(void)
//{
//	printk(KERN_INFO "Goodbye from k_array_test.\n");
//}

