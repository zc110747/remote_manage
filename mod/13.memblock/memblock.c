#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/blkdev.h>
#include <linux/bio.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/hdreg.h>

int ram_blk_major;
struct gendisk * ram_gendisk;
spinlock_t ram_blk_lock;
char *ram_blk_addr = NULL;
char *ram_blk_sursor=NULL;

static void ram_blk_transfer(unsigned long sector, unsigned long nsect, char *buffer, int write)
{
    unsigned long offset = sector << SECTOR_SHIFT;
    unsigned long nbytes = nsect << SECTOR_SHIFT;
    char *io;

    spin_lock(&ram_blk_lock);

    ram_blk_sursor = ram_blk_addr;
    ram_blk_sursor += offset;
    READ_ONCE(*buffer);

    if (write)
        io = memcpy(ram_blk_sursor, buffer, nbytes);
    else
        io = memcpy(buffer, ram_blk_sursor, nbytes);
    if (!io) 
    {
        printk(KERN_ERR"ram disk failed\n");
    }

    spin_unlock(&ram_blk_lock);
}

static void ram_blk_submit_bio(struct bio *bio)
{
    struct bio_vec bvec;
    struct bvec_iter iter;
    sector_t sector = bio->bi_iter.bi_sector;

    bio_for_each_segment(bvec, bio, iter) 
    {
        char *buffer = kmap_atomic(bvec.bv_page) + bvec.bv_offset;
        unsigned len = bvec.bv_len >> SECTOR_SHIFT;

        ram_blk_transfer(sector, len, buffer,
        bio_data_dir(bio) == WRITE);
        sector += len;
        kunmap_atomic(buffer);
    }
    bio_endio(bio);
}

static int ram_blk_open(struct block_device *bdev, fmode_t mode)
{
    return 0;
}

static void ram_blk_release(struct gendisk *disk, fmode_t mode)
{

}

static int ram_blk_getgeo(struct block_device *bdev, struct hd_geometry *geo)
{
    geo->heads = 1;
    geo->sectors = get_capacity(ram_gendisk);
    geo->cylinders = 1;
    return 0;
}

static int ram_blk_ioctl(struct block_device *bdev,fmode_t mode, unsigned int cmd, unsigned long arg)
{
    return 0;
}

static const struct block_device_operations ram_blk_ops = {
    .owner = THIS_MODULE,
    .submit_bio = ram_blk_submit_bio,
    .open = ram_blk_open,
    .release = ram_blk_release,
    .getgeo = ram_blk_getgeo,
    .ioctl = ram_blk_ioctl,
};

static int __init ram_blk_init(void)
{
    int ret;

    /* 注册一个块设备，返回值为主设备号 */
    ram_blk_major = register_blkdev(0, "ram_blk");
    if ( ram_blk_major < 0)
    {
        printk(KERN_ERR"ram disk register failed!\n");
        return -1;
    }

    ram_gendisk = blk_alloc_disk(NUMA_NO_NODE);//分配一个 gendisk
    if (!ram_gendisk)
    {
        printk(KERN_ERR"alloc_disk failed!\n");
        return -1;
    }
    strcpy(ram_gendisk->disk_name, "ram_blk");
    ram_gendisk->major = ram_blk_major;                 //设置主设备号
    ram_gendisk->first_minor = 0;                       //设置第一个分区的次设备号
    ram_gendisk->minors = 1;                            //设置分区个数： 1
    ram_gendisk->fops = &ram_blk_ops;                   //指定块设备 ops 集合
    set_capacity(ram_gendisk, 20480);                   //设置扇区数量： 10MiB/512B=20480
    
    ret = add_disk(ram_gendisk);                        //添加硬盘
    if (!ret)
    {
        printk(KERN_ERR"alloc_disk failed!\n");
        return -1;
    }

    spin_lock_init(&ram_blk_lock);                      //初始化自旋锁

    ram_blk_addr = (char*)vmalloc(10*2048*512); //分配 10M 空间作为硬盘
    if(ram_blk_addr == NULL)
    {
        printk(KERN_ERR"alloc memory failed!\n");
        return -1;
    }
    ram_blk_sursor = ram_blk_addr;
    return 0;
}
static void __exit ram_blk_exit(void)
{
    del_gendisk(ram_gendisk);                       //删除硬盘
    put_disk(ram_gendisk);                          //删除所有未完成的请求
    unregister_blkdev(ram_blk_major, "ram_blk");    //注销块设备
}

module_init(ram_blk_init);
module_exit(ram_blk_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("1477153217@qq.com");                 //作者
MODULE_VERSION("0.1");                              //版本
MODULE_DESCRIPTION("mem_disk");                     //简单的描述