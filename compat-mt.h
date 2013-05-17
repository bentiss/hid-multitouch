#ifndef _COMPAT_MT_H
#define _COMPAT_MT_H

/*
 * Input Multitouch Library (compat version)
 *
 * Copyright (c) 2010 Henrik Rydberg
 * Copyright (c) 2013 Red Hat, Inc
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */

#include "compat-input.h"

#define TRKID_MAX	0xffff

#define INPUT_MT_POINTER	0x0001	/* pointer device, e.g. trackpad */
#define INPUT_MT_DIRECT		0x0002	/* direct device, e.g. touchscreen */
#define INPUT_MT_DROP_UNUSED	0x0004	/* drop contacts not seen in frame */
#define INPUT_MT_TRACK		0x0008	/* use in-kernel tracking */
#define INPUT_MT_SEMI_MT	0x0010	/* semi-mt device, finger count handled manually */

/**
 * struct input_mt_slot - represents the state of an input MT slot
 * @abs: holds current values of ABS_MT axes for this slot
 * @frame: last frame at which input_mt_report_slot_state() was called
 * @key: optional driver designation of this slot
 */
struct input_mt_slot {
	int abs[ABS_MT_LAST - ABS_MT_FIRST + 1];
	unsigned int frame;
	unsigned int key;
};

/**
 * struct input_mt - state of tracked contacts
 * @trkid: stores MT tracking ID for the next contact
 * @num_slots: number of MT slots the device uses
 * @slot: MT slot currently being transmitted
 * @flags: input_mt operation flags
 * @frame: increases every time input_mt_sync_frame() is called
 * @red: reduced cost matrix for in-kernel tracking
 * @slots: array of slots holding current values of tracked contacts
 */
struct input_mt {
	int trkid;
	int num_slots;
	int slot;
	unsigned int flags;
	unsigned int frame;
	int *red;

	/** compat */
	unsigned int num_vals;
	unsigned int max_vals;
	struct input_value *vals;
	/** end of compat */

	struct input_mt_slot slots[];
};

static inline void input_mt_set_value(struct input_mt_slot *slot,
				      unsigned code, int value)
{
	slot->abs[code - ABS_MT_FIRST] = value;
}

static inline int input_mt_get_value(const struct input_mt_slot *slot,
				     unsigned code)
{
	return slot->abs[code - ABS_MT_FIRST];
}

static inline bool input_mt_is_active(const struct input_mt_slot *slot)
{
	return input_mt_get_value(slot, ABS_MT_TRACKING_ID) >= 0;
}

static inline bool input_mt_is_used(const struct input_mt *mt,
				    const struct input_mt_slot *slot)
{
	return slot->frame == mt->frame;
}

int __compat_input_mt_init_slots(struct input_dev *dev, struct input_mt **p_mt,
			unsigned int num_slots, unsigned int flags);
void __compat_input_mt_destroy_slots(struct input_dev *dev, struct input_mt *mt);

static inline int input_mt_new_trkid(struct input_mt *mt)
{
	return mt->trkid++ & TRKID_MAX;
}

void __compat_input_mt_slot(struct input_dev *dev, struct input_mt *mt, int slot);

static inline bool input_is_mt_value(int axis)
{
	return axis >= ABS_MT_FIRST && axis <= ABS_MT_LAST;
}

static inline bool input_is_mt_axis(int axis)
{
	return axis == ABS_MT_SLOT || input_is_mt_value(axis);
}

void __compat_input_mt_report_slot_state(struct input_dev *dev, struct input_mt *mt,
				unsigned int tool_type, bool active);

void __compat_input_mt_report_finger_count(struct input_dev *dev, struct input_mt *mt, int count);
void __compat_input_mt_report_pointer_emulation(struct input_dev *dev,
				       struct input_mt *mt, bool use_count);

void __compat_input_mt_sync_frame(struct input_dev *dev, struct input_mt *mt);

/**
 * struct input_mt_pos - contact position
 * @x: horizontal coordinate
 * @y: vertical coordinate
 */
struct input_mt_pos {
	s16 x, y;
};

int __compat_input_mt_assign_slots(struct input_dev *dev, struct input_mt *mt, int *slots,
			  const struct input_mt_pos *pos, int num_pos);

int __compat_input_mt_get_slot_by_key(struct input_dev *dev, struct input_mt *mt, int key);

#define input_mt_assign_slots(a, b, c, d, e) \
	__compat_input_mt_assign_slots((a), (b), (c), (d), (e))
#define input_mt_get_slot_by_key(a, b, c) \
	__compat_input_mt_get_slot_by_key((a), (b), (c))
#define input_mt_init_slots(a, b, c, d) \
	__compat_input_mt_init_slots((a), (b), (c), (d))
#define input_mt_destroy_slots(a, b) \
	__compat_input_mt_destroy_slots((a), (b))
#define input_mt_slot(a, b, c) \
	__compat_input_mt_slot((a), (b), (c))
#define input_mt_report_slot_state(a, b, c, d) \
	__compat_input_mt_report_slot_state((a), (b), (c), (d))
#define input_mt_report_finger_count(a, b, c) \
	__compat_input_mt_report_finger_count((a), (b), (c))
#define input_mt_report_pointer_emulation(a, b, c) \
	__compat_input_mt_report_pointer_emulation((a), (b), (c))
#define input_mt_sync_frame(a, b) \
	__compat_input_mt_sync_frame((a), (b))
#define input_mt_assign_slots(a, b, c, d, e) \
	__compat_input_mt_assign_slots((a), (b), (c), (d), (e))
#define input_mt_get_slot_by_key(a, b, c) \
	__compat_input_mt_get_slot_by_key((a), (b), (c))

#endif
