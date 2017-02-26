/*
  *  sync_timer.c
  *
  *  Copyright (C) 2017 Rostislav Stepanov <stepanovr@yahoo.com>, All Rights Reserved.
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License version 2 as
  * published by the Free Software Foundation.
  */

#include <linux/device.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/hrtimer.h>
#include <linux/sched.h>
#include "dahdi/kernel.h"


#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>  /* miscellaneous device with Major Number = 10 */

#include "timer.h"

#define DRVNAME		"sync_timer"
#define DEVICE_NAME	"sync_timer"


int dahdi_register_device(struct dahdi_device *ddev, struct device *parent);

static const struct dahdi_span_ops sync_timer_ops = {
        .owner = THIS_MODULE,
};

static sync_dahdi_t dahdi;

static struct hrtimer htimer;

static ktime_t kt_time;
static timer_control tim_control = {
	.seconds = 0,
	.nanoseconds = ((1000000 - 4) * (DAHDI_CHUNKSIZE/8))
};

//static timer_data ret_data = {0};


#define MAX_PINS 32             /* 64 later, when known ok */


static DECLARE_WAIT_QUEUE_HEAD(wq);
static int flag = 0;


/*============================================================================
 * Register interface into Dahdi/Zaptel.
 *
 */
int dahdi_setup_init(sync_dahdi_t *dahdi)
{
        int err;
        /* Zapata stuff */
        dahdi->chan = &dahdi->_chan;
        dahdi->chan_ptrs[0] = &dahdi->_chan;

        sprintf(dahdi->span.name, "SYNCTIMER/1");
        snprintf(       dahdi->span.desc,
                        sizeof(dahdi->span.desc) - 1,
                        "%s (source: " "sync timer" ") %d",
                        dahdi->span.name, 1);
	dahdi->span.spantype = SPANTYPE_ANALOG_FXO;
        sprintf(dahdi->chan->name, "SYNCTIMER/%d/%d", 1, 0);
        dahdi->ddev = dahdi_create_device();
        dev_set_name(&dahdi->ddev->dev, "voicesync");
        dahdi->ddev->manufacturer = "XMOBEX";
	dahdi->ddev->devicetype = "XMOBEX Sync timer";
	dahdi->ddev->location = kasprintf(GFP_KERNEL, "Inner Linux source");


        dahdi->chan->chanpos    = 1;
#if defined(__DAHDI__)
        dahdi->span.chans       = dahdi->chan_ptrs;
#else
        dahdi->span.chans       = dahdi->chan;
#endif
        dahdi->span.channels    = 0;            /* no channels on our span */
        dahdi->span.deflaw      = DAHDI_LAW_MULAW;
        dahdi->chan->pvt        = dahdi;
        dahdi->span.ops         = &sync_timer_ops;

        list_add_tail(&dahdi->span.device_node, &dahdi->ddev->spans);
        err = dahdi_register_device(dahdi->ddev, NULL);
        if (err) {

                return -1;
        }
        return 0;
}


static const struct file_operations timer_fileops = {
	.owner   = THIS_MODULE,
};

static struct miscdevice xbx_dev = {
        minor:  MISC_DYNAMIC_MINOR,
        name:   DRVNAME,
        fops:   &timer_fileops,
};


static void timer_init(uint32_t sec, uint32_t nanosec);



static enum hrtimer_restart timer_function(struct hrtimer * timer)
{
static uint32_t counter = 10000;
if(--counter == 0){
	printk(KERN_ERR DRVNAME ": %s  %d %d\n", __func__, (uint32_t)jiffies, HZ);
	counter = 10000;
}

//	ret_data.order_no++;              /* each timer shot increments this value */
	hrtimer_forward_now(timer, kt_time);
	wake_up_interruptible(&wq);
	flag = 1;
	dahdi_receive(&dahdi.span);
	dahdi_transmit(&dahdi.span);

	return HRTIMER_RESTART;
}


static void timer_init(uint32_t sec, uint32_t nanosec)
{
	kt_time = ktime_set(sec, nanosec); //seconds,nanoseconds
	hrtimer_init (& htimer, CLOCK_REALTIME, HRTIMER_MODE_REL);
	htimer.function = timer_function;
	hrtimer_start(& htimer, kt_time, HRTIMER_MODE_REL);
	//printk(KERN_ERR DRVNAME ": %s  frequency: %d\n", __func__, HZ);

}

static void timer_cleanup(void)
{
	hrtimer_cancel(& htimer);
}

static int __init timer_driver_init(void)
{

	if (misc_register(&xbx_dev)) {
		printk(KERN_DEBUG "%s: Unable to register misc device\n", DRVNAME);
		return -EINVAL;
	} else {
                printk(KERN_INFO "%s: Misc device registered\n", DRVNAME);
		printk(KERN_INFO "%s: Version: %s\n", DRVNAME, SYNC_TIMER_VERSION);

	}

	timer_init(tim_control.seconds, tim_control.nanoseconds);
        if (dahdi_setup_init(&dahdi)){
		timer_cleanup();
		misc_deregister(&xbx_dev);
                return -EINVAL;
        }

	printk(KERN_INFO "%s started\n", DRVNAME);
	return 0;
}

static void __exit timer_driver_cleanup(void){
	timer_cleanup();
	misc_deregister(&xbx_dev);


	dahdi_unregister_device(dahdi.ddev);
	dahdi_free_device(dahdi.ddev);
	printk(KERN_INFO "%s stopped\n", DRVNAME);
}


module_init(timer_driver_init);
module_exit(timer_driver_cleanup);

#if !(DAHDI_CHUNKSIZE==8 || DAHDI_CHUNKSIZE==16 || DAHDI_CHUNKSIZE==40 || DAHDI_CHUNKSIZE==80)
# error Sorry, chunksize must be 8/16/40/80
#endif



MODULE_AUTHOR("Rostislav Stepanov <stepanovr@yahoo.com>");
MODULE_DESCRIPTION("Dahdi syncronization driver");
MODULE_LICENSE("GPL");

