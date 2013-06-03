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

#include <linux/backport.h>

#define input_allocate_device		LINUX_BACKPORT(input_allocate_device)
#define input_register_device		LINUX_BACKPORT(input_register_device)
#define input_unregister_device		LINUX_BACKPORT(input_unregister_device)
#define input_event			LINUX_BACKPORT(input_event)

#include <linux/input.h>
#include <linux/version.h>

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
#endif

#ifndef ABS_MT_DISTANCE
#define ABS_MT_DISTANCE		0x3b	/* Contact hover distance */
#endif

#ifndef ABS_MT_TOOL_X
#define ABS_MT_TOOL_X		0x3c	/* Center X tool position */
#define ABS_MT_TOOL_Y		0x3d	/* Center Y tool position */
#endif

/* Implementation details, userspace should not care about these */
#ifdef ABS_MT_FIRST
#undef ABS_MT_FIRST
#endif
#define ABS_MT_FIRST		ABS_MT_TOUCH_MAJOR
#ifdef ABS_MT_LAST
#undef ABS_MT_LAST
#endif
#define ABS_MT_LAST		ABS_MT_TOOL_Y

#ifndef BTN_TOOL_QUINTTAP
#define BTN_TOOL_QUINTTAP	0x148	/* Five fingers on trackpad */
#endif

#ifndef KEY_IMAGES
#define KEY_IMAGES		0x1ba	/* AL Image Browser */
#endif

#ifndef BTN_TRIGGER_HAPPY
#define BTN_TRIGGER_HAPPY		0x2c0
#define BTN_TRIGGER_HAPPY1		0x2c0
#define BTN_TRIGGER_HAPPY2		0x2c1
#define BTN_TRIGGER_HAPPY3		0x2c2
#define BTN_TRIGGER_HAPPY4		0x2c3
#define BTN_TRIGGER_HAPPY5		0x2c4
#define BTN_TRIGGER_HAPPY6		0x2c5
#define BTN_TRIGGER_HAPPY7		0x2c6
#define BTN_TRIGGER_HAPPY8		0x2c7
#define BTN_TRIGGER_HAPPY9		0x2c8
#define BTN_TRIGGER_HAPPY10		0x2c9
#define BTN_TRIGGER_HAPPY11		0x2ca
#define BTN_TRIGGER_HAPPY12		0x2cb
#define BTN_TRIGGER_HAPPY13		0x2cc
#define BTN_TRIGGER_HAPPY14		0x2cd
#define BTN_TRIGGER_HAPPY15		0x2ce
#define BTN_TRIGGER_HAPPY16		0x2cf
#define BTN_TRIGGER_HAPPY17		0x2d0
#define BTN_TRIGGER_HAPPY18		0x2d1
#define BTN_TRIGGER_HAPPY19		0x2d2
#define BTN_TRIGGER_HAPPY20		0x2d3
#define BTN_TRIGGER_HAPPY21		0x2d4
#define BTN_TRIGGER_HAPPY22		0x2d5
#define BTN_TRIGGER_HAPPY23		0x2d6
#define BTN_TRIGGER_HAPPY24		0x2d7
#define BTN_TRIGGER_HAPPY25		0x2d8
#define BTN_TRIGGER_HAPPY26		0x2d9
#define BTN_TRIGGER_HAPPY27		0x2da
#define BTN_TRIGGER_HAPPY28		0x2db
#define BTN_TRIGGER_HAPPY29		0x2dc
#define BTN_TRIGGER_HAPPY30		0x2dd
#define BTN_TRIGGER_HAPPY31		0x2de
#define BTN_TRIGGER_HAPPY32		0x2df
#define BTN_TRIGGER_HAPPY33		0x2e0
#define BTN_TRIGGER_HAPPY34		0x2e1
#define BTN_TRIGGER_HAPPY35		0x2e2
#define BTN_TRIGGER_HAPPY36		0x2e3
#define BTN_TRIGGER_HAPPY37		0x2e4
#define BTN_TRIGGER_HAPPY38		0x2e5
#define BTN_TRIGGER_HAPPY39		0x2e6
#define BTN_TRIGGER_HAPPY40		0x2e7

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
 * @input: placeholder of the input_dev
 * @mt: pointer to multitouch state
 * @num_vals: number of values queued in the current frame
 * @max_vals: maximum number of values queued in a frame
 * @vals: array of values queued in the current frame
 */
struct __compat_input_dev {
	struct input_dev input;
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
	return container_of(dev, struct __compat_input_dev, input);
}

static inline struct input_mt *input_get_mt(struct input_dev *dev)
{
	struct __compat_input_dev *_dev = __input_to_compat(dev);
	if (_dev)
		return _dev->mt;
	return NULL;
}

static inline void input_set_mt(struct input_dev *dev, struct input_mt *mt)
{
	struct __compat_input_dev *_dev = __input_to_compat(dev);
	if (_dev)
		_dev->mt = mt;
}

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
