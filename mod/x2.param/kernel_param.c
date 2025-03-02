////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2024-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      kernel_param.c
//
//  Purpose:
//      
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

static char* test_str="hello world!";
module_param(test_str, charp, 0644);
static bool bval = true;
module_param(bval, bool, S_IRUGO);
static int array_list[2] = {1, 2};
module_param_array(array_list, int, NULL, S_IRUGO);

static int __init kernel_module_init(void)
{
    printk(KERN_INFO"test_str:%s\n", test_str);
    printk(KERN_INFO"bval:%d\n", bval);
    printk(KERN_INFO"array[0]=%d\n", array_list[0]);
    return 0;
}

static void __exit kernel_module_exit(void)
{

}

module_init(kernel_module_init);
module_exit(kernel_module_exit);
MODULE_AUTHOR("wzdxf");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("driver for param");
MODULE_ALIAS("kernel param");
