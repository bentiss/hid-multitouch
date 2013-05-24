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
#ifndef _COMPAT_INPUT_H
#define _COMPAT_INPUT_H

#include <linux/input.h>
#include <linux/version.h>
#include "compat-mt.h"

#ifndef ABS_MT_SLOT
#define ABS_MT_SLOT		0x2f	/* MT slot being modified */
#define ABS_MT_TOUCH_MAJOR	0x30	/* Major axis of touching ellipse */
#define ABS_MT_TOUCH_MINOR	0x31	/* Minor axis (omit if circular) */
#define ABS_MT_WIDTH_MAJOR	0x32	/* Major axis of approaching ellipse */
#define ABS_MT_WIDTH_MINOR	0x33	/* Minor axis (omit if circular) */
#define ABS_MT_ORIENTATION	0x34	/* Ellipse orientation */
#define ABS_MT_POSITION_X	0x35	/* Center X touch position */
#define ABS_MT_POSITION_Y	0x36	/* Center Y touch position */
#define ABS_MT_TOOL_TYPE	0x37	/* Type of touching device */
#define ABS_MT_BLOB_ID		0x38	/* Group a set of packets as a blob */
#define ABS_MT_TRACKING_ID	0x39	/* Unique ID of initiated contact */
#define ABS_MT_PRESSURE		0x3a	/* Pressure on contact area */

/* Implementation details, userspace should not care about these */
#define ABS_MT_FIRST		ABS_MT_TOUCH_MAJOR
#define ABS_MT_LAST		ABS_MT_TOOL_Y
#endif

#ifndef ABS_MT_DISTANCE
#define ABS_MT_DISTANCE		0x3b	/* Contact hover distance */
#endif

#ifndef ABS_MT_TOOL_X
#define ABS_MT_TOOL_X		0x3c	/* Center X tool position */
#define ABS_MT_TOOL_Y		0x3d	/* Center Y tool position */
#endif

#ifndef BTN_TOOL_QUINTTAP
#define BTN_TOOL_QUINTTAP	0x148	/* Five fingers on trackpad */
#endif

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
 * struct __compat_input_dev - represents an input device
 * @mt: pointer to multitouch state
 * @num_vals: number of values queued in the current frame
 * @max_vals: maximum number of values queued in a frame
 * @vals: array of values queued in the current frame
 */
struct __compat_input_dev {
	struct input_mt *mt;

	unsigned int num_vals;
	unsigned int max_vals;
	struct input_value *vals;

	/* private */
	void *p;
	void *p1;
};

static inline struct __compat_input_dev *__input_to_compat(struct input_dev *dev)
{
	return (struct __compat_input_dev *)dev->dev.platform_data;
}

struct __compat_input_dev *input_allocate_extra(struct input_dev *dev);
void input_free_extra(struct input_dev *dev);
struct input_mt *input_get_mt(struct input_dev *dev);
void input_set_mt(struct input_dev *dev, struct input_mt *mt);

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
void __compat_input_event(struct input_dev *dev,
		 unsigned int type, unsigned int code, int value);

static inline void __compat_input_sync(struct input_dev *dev)
{
	__compat_input_event(dev, EV_SYN, SYN_REPORT, 0);
}

#define input_event(a, b, c, d) \
	__compat_input_event((a), (b), (c), (d))
#define input_sync(a) \
	__compat_input_sync((a))

#ifndef kstrtoul
#define kstrtoul strict_strtoul
#endif

#ifndef INPUT_GENERATE_ABS_ACCESSORS
static inline int input_abs_get_val(struct input_dev *dev,
					  unsigned int axis)
{
	return dev->abs[axis];
}

static inline void input_abs_set_val(struct input_dev *dev,
					   unsigned int axis, int val)
{
	dev->abs[axis] = val;
}

#define INPUT_GENERATE_ABS_ACCESSORS(_suffix, _item)			\
static inline int input_abs_get_##_suffix(struct input_dev *dev,	\
					  unsigned int axis)		\
{									\
	return dev->abs##_suffix[axis];					\
}									\
									\
static inline void input_abs_set_##_suffix(struct input_dev *dev,	\
					   unsigned int axis, int val)	\
{									\
	dev->abs##_suffix[axis] = val;				\
}

INPUT_GENERATE_ABS_ACCESSORS(min, minimum)
INPUT_GENERATE_ABS_ACCESSORS(max, maximum)
INPUT_GENERATE_ABS_ACCESSORS(fuzz, fuzz)
INPUT_GENERATE_ABS_ACCESSORS(flat, flat)
INPUT_GENERATE_ABS_ACCESSORS(res, resolution)
#endif

#endif
