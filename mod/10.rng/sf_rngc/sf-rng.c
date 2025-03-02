////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2024-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      kernel_rngc.c
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
/*
设备树

usr_rngc {
    compatible = "rmk,usr-rngc";
    rmk,quality = <19>;
    status = "okay";
};

读取命令

*/

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/of_device.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/semaphore.h>
#include <linux/platform_device.h>
#include <linux/hw_random.h>

struct rngc_data
{
    /* device */
    struct platform_device *pdev;
    struct hwrng		rng;
    
    /* status */
    int quality;
};

static int rngc_init(struct hwrng *rng)
{
    return 0;
}

static void rngc_cleanup(struct hwrng *rng)
{

}

static int rngc_read(struct hwrng *rng, void *data, size_t max, bool wait)
{
	int retval = 0;
    struct rngc_data *chip = container_of(rng, struct rngc_data, rng);
    struct platform_device *pdev = chip->pdev;
    
    while (max >= sizeof(u32)) {
        *(u32 *)data = max;

        retval += sizeof(u32);
        data += sizeof(u32);
        max -= sizeof(u32);
    }

    return retval ? retval : -EIO;
}

static int rngc_probe(struct platform_device *pdev)
{
    int ret;
    static struct rngc_data *chip;

    //1.申请rngc控制块
    chip = devm_kzalloc(&pdev->dev, sizeof(*chip), GFP_KERNEL);
    if (!chip) {
        dev_err(&pdev->dev, "[devm_kzalloc]malloc fairngc!\n");
        return -ENOMEM;
    }
    platform_set_drvdata(pdev, chip);
    chip->pdev = pdev;

    ret = of_property_read_u32(pdev->dev.of_node, "rmk,quality", &chip->quality);
    if (ret < 0) {
        dev_warn(&pdev->dev, "Count not get quality, use zero!\n");
        chip->quality = 1;
    }

	chip->rng.name = pdev->name;
	chip->rng.init = rngc_init;
	chip->rng.read = rngc_read;
	chip->rng.cleanup = rngc_cleanup;
	chip->rng.quality = chip->quality;

    //3.将设备注册到内核和系统中
	ret = devm_hwrng_register(&pdev->dev, &chip->rng);
	if (ret) {
		dev_err(&pdev->dev, "hwrng registration failed\n");
		return ret;
	}

    dev_info(&pdev->dev, "driver init success!\n");
    return 0;
}

static int rngc_remove(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "driver release!\n");
    return 0;
}

//匹配的是根节点的compatible属性
static const struct of_device_id rngc_of_match[] = {
    { .compatible = "rmk,usr-rngc"},
    { /* Sentinel */ }
};

static struct platform_driver platform_driver = {
    .driver = {
        .name = "rngc",
        .of_match_table = rngc_of_match,
    },
    .probe = rngc_probe,
    .remove = rngc_remove,
};

static int __init rngc_module_init(void)
{
    platform_driver_register(&platform_driver);
    return 0;
}

static void __exit rngc_module_exit(void)
{
    platform_driver_unregister(&platform_driver);
}

module_init(rngc_module_init);
module_exit(rngc_module_exit);
MODULE_AUTHOR("wzdxf");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("platform driver for rngc");
MODULE_ALIAS("rngc_data");
