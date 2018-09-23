#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/slab.h>		// includes kmalloc type flags
#include <linux/cdev.h>
#include <linux/list.h>
#include <asm/uaccess.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/moduleparam.h>

#define MYDEV_NAME "mycdrv"
#define ramdisk_size (size_t) (16 * PAGE_SIZE) // ramdisk size 
#define CDEV_IOC_MAGIC 'k'

#define asp_clear_buffer _IO(CDEV_IOC_MAGIC, 1)

//NUM_DEVICES defaults to 3 unless specified during insmod
static int NUM_DEVICES = 3;


struct asp_mycdrv 
{
	struct cdev cdev;
	char *ramdisk;
	unsigned long buf_size;
	struct semaphore sem;
	int devNo;
	struct list_head list;
};
//NUM_DEVICES can't be manipulated

module_param(NUM_DEVICES, int, S_IRUGO);
static unsigned int majornum = 0;
static struct class *dev_class = NULL;
LIST_HEAD(dev_head);     // Kernel link list intialized

int mycdrv_open(struct inode *inode, struct file *fp)
{
        unsigned int mjr = imajor(inode);
        unsigned int mnr = iminor(inode);
		struct list_head *current_ptr = NULL;
        struct asp_mycdrv *dev = NULL;
//Check for device information by major and minor numbers
	if (mjr != majornum || mnr < 0 || mnr >= NUM_DEVICES) 
		{
                printk("No device found with major=%d and minor=%d\n", mjr, mnr);
                return -ENODEV;
        }

	list_for_each(current_ptr, &dev_head)
	{
		dev = list_entry(current_ptr, struct asp_mycdrv, list);
		if(dev->devNo == mnr) // list reaches to the end of the devices
		{
			break;
		}
	}

        fp->private_data = dev; 

        return 0;
}

loff_t mycdrv_lseek(struct file *fp, loff_t off, int cursor)
{
	struct asp_mycdrv *dev = (struct asp_mycdrv *)fp->private_data;
	loff_t new_pst = 0;       
	unsigned char* temp;
	if (down_interruptible(&(dev->sem))!=0) {
		pr_err("%s: could not lock during open\n", MYDEV_NAME);
	}

	switch(cursor)
	 {
		case SEEK_SET: /* Begining of the file */
			new_pst = off;
			break;

		case SEEK_CUR: 
			new_pst = fp->f_pos + off;
			break;

		case SEEK_END: 
			new_pst = dev->buf_size + off;
			break;
	}
	
	if (new_pst > dev->buf_size)
	{
		//figuring out appropriate buffer size
		temp = (unsigned char*)kzalloc(new_pst,GFP_KERNEL);
		memcpy(temp, dev->ramdisk, dev->buf_size);


		kfree(dev->ramdisk);
		dev->ramdisk = temp;
		dev->buf_size = new_pst;
	}

	fp->f_pos = new_pst;

	up(&(dev->sem));
	return new_pst;
}

int mycdrv_release(struct inode *inode, struct file *fp)
{
        return 0;
}

ssize_t mycdrv_read(struct file *fp, char __user *buf, size_t counter,loff_t *f_pos)
{
	struct asp_mycdrv *dev = (struct asp_mycdrv *)fp->private_data;
	ssize_t ret = 0;
	unsigned char *temp_buff = NULL;
	loff_t i;

	if (down_interruptible(&(dev->sem))!=0) 
	{
		pr_err("%s: Could lock during open\n", MYDEV_NAME);  //could not acquire lock as other api's are using it
	}

	if (*f_pos >= dev->buf_size) /* End Of File */
		goto label;

	temp_buff = (unsigned char*) kzalloc(counter, GFP_KERNEL);	
	if (temp_buff == NULL) 
	{
		pr_err("%s: Out of Memory -- Operation Failed", __func__);
		ret = -ENOMEM;
		goto label;
	}

	for (i = 0; i < counter; i++)
	{
		temp_buff[i] = dev->ramdisk[*f_pos + i];
	}
	
	*f_pos += counter;

	if (copy_to_user(buf, temp_buff, counter) != 0)
	{
		ret = -EFAULT;
		goto label;
	}

	ret = counter;

label:
	if (temp_buff != NULL)
		kfree(temp_buff);
	up(&(dev->sem));
	return ret;
}

ssize_t mycdrv_write(struct file *fp, const char __user *buf, size_t counter, 
		loff_t *f_pos)
{
	struct asp_mycdrv *dev = (struct asp_mycdrv *)fp->private_data;
	ssize_t ret = 0;
	unsigned char *temp_buff = NULL;
	loff_t i;

	if (down_interruptible(&(dev->sem))!=0) {
		pr_err("%s:Could Not Lock During Open\n", MYDEV_NAME);
	} 

	temp_buff = (unsigned char*) kzalloc(counter, GFP_KERNEL);	
	if (temp_buff == NULL) {
		pr_err("%s:Out of Memory Operation Failed", __func__);
		ret = -ENOMEM;
		goto label;
	}

	if (copy_from_user(temp_buff, buf, counter) != 0)
	{
		ret = -EFAULT;
		goto label;
	}

	for (i = 0; i < counter; i++)
	{
		dev->ramdisk[*f_pos + i] = temp_buff[i];
	}
	*f_pos += counter;

	ret = counter;
label:
	if (temp_buff != NULL)
		kfree(temp_buff);

	up(&(dev->sem));
	return ret;
}

long mycdrv_ioctl (struct file *fp, unsigned int cmd, unsigned long arg)
{
    struct asp_mycdrv *dev = (struct asp_mycdrv *)fp->private_data;
	long ret;

	if (cmd != asp_clear_buffer) 
	{
		pr_err("command: %d: act_command: %d\n", cmd, asp_clear_buffer);
		return -1;
	}

	if (down_interruptible(&(dev->sem))!=0) {
		pr_err("%s: could not lock during open\n", MYDEV_NAME);
	} 

	memset((volatile void*)dev->ramdisk,0,dev->buf_size);
	//memset(void*,int,size_t);
	fp->f_pos=0;	

	up(&(dev->sem));
	return 1;
}


struct file_operations mycdrv_fops = {
        .owner =    THIS_MODULE,
        .read =     mycdrv_read,
        .write =    mycdrv_write,
        .open =     mycdrv_open,
        .release =  mycdrv_release,
        .llseek =   mycdrv_lseek,
	.unlocked_ioctl = mycdrv_ioctl,
};

static int mycdrv_create(struct asp_mycdrv *dev, int minor,struct class *class)
{
        dev_t devno = MKDEV(majornum, minor);
        struct device *device = NULL;

        dev->buf_size = ramdisk_size;
		dev->devNo = minor;
		dev->ramdisk = NULL; 
		sema_init(&(dev->sem),1);
        
        cdev_init(&dev->cdev, &mycdrv_fops);
        dev->cdev.owner = THIS_MODULE;

        dev->ramdisk = (unsigned char*)kzalloc(dev->buf_size, GFP_KERNEL);

        cdev_add(&dev->cdev, devno, 1);

        device = device_create(class, NULL, devno, NULL,MYDEV_NAME "%d", minor);

        return 0;
}

static void mycdrv_destroy(void) 	// unregister the devices and clean the resources used 
{
        int i = 0;
        struct list_head *current_ptr = NULL;
        struct asp_mycdrv *dev = NULL;

redo:
	list_for_each(current_ptr, &dev_head) 
	{
		dev = list_entry(current_ptr, struct asp_mycdrv, list);
		device_destroy(dev_class, MKDEV(majornum, i));
        cdev_del(&dev->cdev);
        kfree(dev->ramdisk);
		list_del(&(dev->list));
        kfree(dev);
		i++;
        goto redo;
    }

	if (dev_class)
		class_destroy(dev_class);

	unregister_chrdev_region(MKDEV(majornum, 0), NUM_DEVICES);
}

static int __init my_init(void)
{	
		int n;
        dev_t dev = 0;
        struct asp_mycdrv *char_dev = NULL;

        //devices registered for further use for user space
        alloc_chrdev_region(&dev, 0, NUM_DEVICES, MYDEV_NAME);
        
        majornum = MAJOR(dev);
        dev_class = class_create(THIS_MODULE, MYDEV_NAME);
         
	for (n = 0; n < NUM_DEVICES; ++n) 
        {
		char_dev = (struct asp_mycdrv *)kzalloc(sizeof(struct asp_mycdrv),GFP_KERNEL);
		mycdrv_create(char_dev, n, dev_class);

		INIT_LIST_HEAD(&(char_dev->list));
		list_add(&(char_dev->list), &dev_head);
        }

        return 0;
}

static void __exit my_exit(void)
{
        mycdrv_destroy();
      //  printk(KERN_ALERT DEVICE_NAME": unloaded module");
        //Unregistering in descending order
        return;
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Nirali Patel");
MODULE_LICENSE("GPL v2");
