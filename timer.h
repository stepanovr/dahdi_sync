/*
  *  timer.h
  *
  *  Copyright (C) 2017 Rostislav Stepanov <stepanovr@yahoo.com>, All Rights Reserved.
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License version 2 as
  * published by the Free Software Foundation.
  */

#ifndef	__timer_h__
#define	__timer_h__


typedef struct _timer_control{		/* Timer parameters. This structure is written to setup timer parameters*/
	uint32_t seconds;
	uint32_t nanoseconds;
}timer_control;


typedef struct sync_dahdi
{
        struct dahdi_span       span;
        struct dahdi_chan*      chan;
        struct dahdi_chan*      chan_ptrs[1];
        struct dahdi_chan       _chan;
        struct dahdi_device     *ddev;

} sync_dahdi_t;

#define SYNC_TIMER_VERSION	"0.0.1"

#endif
