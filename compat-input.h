#ifndef _COMPAT_INPUT_H
#define _COMPAT_INPUT_H

/*
 * Input compat Library
 *
 * Copyright (c) 1999-2002 Vojtech Pavlik
 * Copyright (c) 2013 Red Hat, inc
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */

#include <linux/input.h>
#include <linux/version.h>

#ifndef ABS_MT_TOOL_X
#define ABS_MT_TOOL_X		0x3c	/* Center X tool position */
#define ABS_MT_TOOL_Y		0x3d	/* Center Y tool position */
#endif

/**
 * forward declaration of input_mt.
 */
struct input_mt;

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 7, 0)
/**
 * struct input_value - input value representation
 * @type: type of value (EV_KEY, EV_ABS, etc)
 * @code: the value code
 * @value: the value
 */
struct input_value {
	__u16 type;
	__u16 code;
	__s32 value;
};
#endif

/**
 * input_event() - report new input event
 * @dev: device that generated the event
 * @mt: mt slots definition
 * @type: type of the event
 * @code: event code
 * @value: value of the event
 *
 * This function should be used by drivers implementing various input
 * devices to report input events. See also input_inject_event().
 *
 * NOTE: input_event() may be safely used right after input device was
 * allocated with input_allocate_device(), even before it is registered
 * with input_register_device(), but the event will not reach any of the
 * input handlers. Such early invocation of input_event() may be used
 * to 'seed' initial state of a switch or initial position of absolute
 * axis, etc.
 */
void __compat_input_event(struct input_dev *dev, struct input_mt *mt,
		 unsigned int type, unsigned int code, int value);

static inline void __compat_input_sync(struct input_dev *dev, struct input_mt *mt)
{
	__compat_input_event(dev, mt, EV_SYN, SYN_REPORT, 0);
}

#define input_event(a, b, c, d, e) \
	__compat_input_event((a), (b), (c), (d), (e))
#define input_sync(a, b) \
	__compat_input_sync((a), (b))

#endif
