#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/time.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "DiceDev"
#define CLS0 "DiceClass0"
#define CLS1 "DiceClass1"
#define CLS2 "DiceClass2"
#define DEV0 "DiceDev0"
#define DEV1 "DiceDev1"
#define DEV2 "DiceDev2"

MODULE_LICENSE("GPL");

static struct timespec ts;

// write
static int diceNum[3] = {1, 1, 1};
// io temp var
static char wBuffer[8] = {0};
static char convBuffer[64] = {0};
static int gen_sides = 6;
module_param(gen_sides,int,S_IRUGO);

// major number
static int majorNumber;

// device drive class struct ptr
static struct class* diceClass0 = NULL;
static struct class* diceClass1 = NULL;
static struct class* diceClass2 = NULL;

// device drive device struct
static struct cdev diceDev0;
static struct cdev diceDev1;
static struct cdev diceDev2;

static const char* diceShape[6] = {
    "-------\n"
    "|     |\n"
    "|  o  |\n"
    "|     |\n"
    "-------\n",
    "-------\n"
    "|     |\n"
    "| o o |\n"
    "|     |\n"
    "-------\n",
    "-------\n"
    "|  o  |\n"
    "|     |\n"
    "| o o |\n"
    "-------\n",
    "-------\n"
    "| o o |\n"
    "|     |\n"
    "| o o |\n"
    "-------\n",
    "-------\n"
    "| o o |\n"
    "|  o  |\n"
    "| o o |\n"
    "-------\n",
    "-------\n"
    "| o o |\n"
    "| o o |\n"
    "| o o |\n"
    "-------\n"
};

static int gen_rand(int mod) {
    int rd;
    getnstimeofday(&ts);
    rd = ts.tv_nsec % mod;
    return abs(rd);
}

// prototype function 
static int dev_open(struct inode*, struct file*);
static int dev_release(struct inode*, struct file*);
static ssize_t dev_read0(struct file*, char*, size_t, loff_t*);
static ssize_t dev_write0(struct file*, const char*, size_t, loff_t*);
static ssize_t dev_read1(struct file*, char*, size_t, loff_t*);
static ssize_t dev_write1(struct file*, const char*, size_t, loff_t*);
static ssize_t dev_read2(struct file*, char*, size_t, loff_t*);
static ssize_t dev_write2(struct file*, const char*, size_t, loff_t*);


static struct file_operations fops0 = {
    .open = dev_open,
    .read = dev_read0,
    .write = dev_write0,
    .release = dev_release,
};

static struct file_operations fops1 = {
    .open = dev_open,
    .read = dev_read1,
    .write = dev_write1,
    .release = dev_release,
};

static struct file_operations fops2 = {
    .open = dev_open,
    .read = dev_read2,
    .write = dev_write2,
    .release = dev_release,
};

static int __init dice_init(void) {
    dev_t devId;
    printk(KERN_INFO "DICE: module option: gen_sides: %d", gen_sides);
    if (gen_sides <= 0) {
        gen_sides = 6;
    }
    // register three minor devices
    alloc_chrdev_region(&devId, 0, 3, DEVICE_NAME);
    majorNumber = MAJOR(devId);

    cdev_init(&diceDev0, &fops0);
    cdev_add(&diceDev0, MKDEV(majorNumber, 0), 1);

    cdev_init(&diceDev1, &fops1);
    cdev_add(&diceDev1, MKDEV(majorNumber, 1), 1);

    cdev_init(&diceDev2, &fops2);
    cdev_add(&diceDev2, MKDEV(majorNumber, 2), 1);

    // register device class
    diceClass0 = class_create(THIS_MODULE, CLS0);
    diceClass1 = class_create(THIS_MODULE, CLS1);
    diceClass2 = class_create(THIS_MODULE, CLS2);

    // register devices
    device_create(diceClass0, 0, MKDEV(majorNumber, 0), 0, DEV0);
    device_create(diceClass1, 0, MKDEV(majorNumber, 1), 0, DEV1);
    device_create(diceClass2, 0, MKDEV(majorNumber, 2), 0, DEV2);

    printk(KERN_INFO "DICE: device create correctly\n");
    return 0;
}

static void __exit dice_exit(void) {
    // remove device
    device_destroy(diceClass0, MKDEV(majorNumber, 0));
    device_destroy(diceClass1, MKDEV(majorNumber, 1));
    device_destroy(diceClass2, MKDEV(majorNumber, 2));
    class_unregister(diceClass0);
    class_unregister(diceClass1);
    class_unregister(diceClass2);
    class_destroy(diceClass0);
    class_destroy(diceClass1);
    class_destroy(diceClass2);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "DICE: exit\n");
}


// called each time the device is created
static int dev_open(struct inode* inodeptr, struct file* fileptr) {
    printk(KERN_INFO "DICE: device opened\n");
    return 0;
}

static int dev_release(struct inode* inodeptr, struct file* fileptr) {
    printk(KERN_INFO "DICE: device closed\n");
    return 0;
}

static ssize_t dev_read0(struct file* fileptr, char* buffer, size_t len, loff_t* offset) {
    int total_count = 0;
    int i;
    if (*offset > 0) {
        return 0;
    }
    // put message to user
    for (i=0; i<diceNum[0]; i++) {
        int rd = gen_rand(6);
        int oft = 0;
        const char* s = diceShape[rd];
        while (s[oft] != 0) {
            put_user(s[oft], buffer++);
            oft++;
            (*offset)++;
        }
        total_count += strlen(s);
    }
    return total_count;
}

static ssize_t dev_read1(struct file* fileptr, char* buffer, size_t len, loff_t* offset) {
    int total_count = 0;
    int i;
    if (*offset > 0) {
        return 0;
    }
    // put message to user
    for (i=0; i<diceNum[1]; i++) {
        int rd = gen_rand(6)+1;
        int oft = 0;
        sprintf(convBuffer, "%d ", rd);
        while (convBuffer[oft] != 0) {
            put_user(convBuffer[oft], buffer++);
            oft++;
            (*offset)++;
        }
        total_count += strlen(convBuffer);
    }
    put_user('\n', buffer++);
    total_count++;
    return total_count;
}

static ssize_t dev_read2(struct file* fileptr, char* buffer, size_t len, loff_t* offset) {
    int total_count = 0;
    int i;
    if (*offset > 0) {
        return 0;
    }
    // put message to user
    for (i=0; i<diceNum[2]; i++) {
        int rd = gen_rand(gen_sides)+1;
        int oft = 0;
        sprintf(convBuffer, "%d ", rd);
        while (convBuffer[oft] != 0) {
            put_user(convBuffer[oft], buffer++);
            oft++;
            (*offset)++;
        }
        total_count += strlen(convBuffer);
    }
    // new line
    put_user('\n', buffer++);
    total_count++;
    return total_count;
}

static ssize_t dev_write0(struct file* fileptr, const char* buffer, size_t len, loff_t* offset) {
    long t;
    int i = 0;
    while (i<len) {
        get_user(wBuffer[i], buffer+i);
        i++;
    }
    kstrtoul(wBuffer, 10, &t);
    diceNum[0] = (int)t;
    printk(KERN_INFO "DICE: write dice number %d to %d th dice", (int)t, 0);
    return len;
}

static ssize_t dev_write1(struct file* fileptr, const char* buffer, size_t len, loff_t* offset) {
    long t;
    int i = 0;
    while (i<len) {
        get_user(wBuffer[i], buffer+i);
        i++;
    }
    kstrtoul(wBuffer, 10, &t);
    diceNum[1] = (int)t;
    printk(KERN_INFO "DICE: write dice number %d to %d th dice", (int)t, 1);
    return len;
}

static ssize_t dev_write2(struct file* fileptr, const char* buffer, size_t len, loff_t* offset) {
    long t;
    int i = 0;
    while (i<len) {
        get_user(wBuffer[i], buffer+i);
        i++;
    }
    kstrtoul(wBuffer, 10, &t);
    diceNum[2] = (int)t;
    printk(KERN_INFO "DICE: write dice number %d to %d th dice", (int)t, 2);
    return len;
}

module_init(dice_init);
module_exit(dice_exit);