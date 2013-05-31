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

#include <linux/backport.h>

#define input_mt_init_slots			LINUX_BACKPORT(input_mt_init_slots)
#define input_mt_destroy_slots			LINUX_BACKPORT(input_mt_destroy_slots)
#define input_mt_report_slot_state		LINUX_BACKPORT(input_mt_report_slot_state)
#define input_mt_report_finger_count		LINUX_BACKPORT(input_mt_report_finger_count)
#define input_mt_report_pointer_emulation	LINUX_BACKPORT(input_mt_report_pointer_emulation)
#define input_mt_sync_frame			LINUX_BACKPORT(input_mt_sync_frame)
#define input_mt_assign_slots			LINUX_BACKPORT(input_mt_assign_slots)
#define input_mt_get_slot_by_key		LINUX_BACKPORT(input_mt_get_slot_by_key)

#include <linux/compat-input.h>

#endif
