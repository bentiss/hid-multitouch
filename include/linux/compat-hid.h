/*
 *  Copyright (c) 1999 Andreas Gal
 *  Copyright (c) 2000-2001 Vojtech Pavlik
 *  Copyright (c) 2006-2007 Jiri Kosina
 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Should you need to contact me, the author, you can do so either by
 * e-mail - mail your message to <vojtech@ucw.cz>, or by paper mail:
 * Vojtech Pavlik, Simunkova 1594, Prague 8, 182 00 Czech Republic
 */
#ifndef __COMPAT_HID_H
#define __COMPAT_HID_H

#include <linux/fs.h>
#include <linux/version.h>
#include <linux/backport.h>

#include <linux/usb.h>

#ifndef IS_ENABLED
/*
 * Getting something that works in C and CPP for an arg that may or may
 * not be defined is tricky.  Here, if we have "#define CONFIG_BOOGER 1"
 * we match on the placeholder define, insert the "0," for arg1 and generate
 * the triplet (0, 1, 0).  Then the last step cherry picks the 2nd arg (a one).
 * When CONFIG_BOOGER is not defined, we generate a (... 1, 0) pair, and when
 * the last step cherry picks the 2nd arg, we get a zero.
 */
#define __ARG_PLACEHOLDER_1 0,
#define config_enabled(cfg) _config_enabled(cfg)
#define _config_enabled(value) __config_enabled(__ARG_PLACEHOLDER_##value)
#define __config_enabled(arg1_or_junk) ___config_enabled(arg1_or_junk 1, 0)
#define ___config_enabled(__ignored, val, ...) val

/*
 * IS_ENABLED(CONFIG_FOO) evaluates to 1 if CONFIG_FOO is set to 'y' or 'm',
 * 0 otherwise.
 *
 */
#define IS_ENABLED(option) \
	(config_enabled(option) || config_enabled(option##_MODULE))
#endif /* IS_ENABLED */

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 9, 0)
static inline struct inode *file_inode(struct file *f)
{
	return f->f_path.dentry->d_inode;
}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 35)
#define usb_alloc_coherent	usb_buffer_alloc
#define usb_free_coherent	usb_buffer_free
#endif

#ifndef usb_unblock_urb
#define usb_unblock_urb        usb_unpoison_urb

/**
 * usb_block_urb - reliably prevent further use of an URB
 * @urb: pointer to URB to be blocked, may be NULL
 *
 * After the routine has run, attempts to resubmit the URB will fail
 * with error -EPERM.  Thus even if the URB's completion handler always
 * tries to resubmit, it will not succeed and the URB will become idle.
 *
 * The URB must not be deallocated while this routine is running.  In
 * particular, when a driver calls this routine, it must insure that the
 * completion handler cannot deallocate the URB.
 */
static void inline usb_block_urb(struct urb *urb)
{
	if (!urb)
		return;

	atomic_inc(&urb->reject);
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 34)
static inline void usb_autopm_get_interface_no_resume(
		struct usb_interface *intf)
{
	atomic_inc(&intf->pm_usage_cnt);
}
static inline void usb_autopm_put_interface_no_suspend(
				struct usb_interface *intf)
{
	atomic_dec(&intf->pm_usage_cnt);
}
#endif


#endif

#ifndef PMSG_IS_AUTO
#define PMSG_IS_AUTO(msg)      (((msg).event & PM_EVENT_AUTO) != 0)
#endif

#ifndef HID_BUS_ANY
#define HID_BUS_ANY				0xffff
#define HID_GROUP_ANY				0x0000
#endif

typedef unsigned long compat_kernel_ulong_t;

struct compat_hid_device_id {
	__u16 bus;
	__u16 group;
	__u32 vendor;
	__u32 product;
	compat_kernel_ulong_t driver_data;
};

#ifndef module_driver
/**
 * module_driver() - Helper macro for drivers that don't do anything
 * special in module init/exit. This eliminates a lot of boilerplate.
 * Each module may only use this macro once, and calling it replaces
 * module_init() and module_exit().
 *
 * @__driver: driver name
 * @__register: register function for this driver type
 * @__unregister: unregister function for this driver type
 * @...: Additional arguments to be passed to __register and __unregister.
 *
 * Use this macro to construct bus specific macros for registering
 * drivers, and do not use it on its own.
 */
#define module_driver(__driver, __register, __unregister, ...) \
static int __init __driver##_init(void) \
{ \
	return __register(&(__driver) , ##__VA_ARGS__); \
} \
module_init(__driver##_init); \
static void __exit __driver##_exit(void) \
{ \
	__unregister(&(__driver) , ##__VA_ARGS__); \
} \
module_exit(__driver##_exit);
#endif

/**
 * general hid functions
 */

#define hid_debug			LINUX_BACKPORT(hid_debug)
#define hid_ignore			LINUX_BACKPORT(hid_ignore)
#define hid_add_device			LINUX_BACKPORT(hid_add_device)
#define hid_destroy_device		LINUX_BACKPORT(hid_destroy_device)
#define __hid_register_driver		LINUX_BACKPORT(__hid_register_driver)
#define hid_unregister_driver		LINUX_BACKPORT(hid_unregister_driver)
#define hidinput_hid_event		LINUX_BACKPORT(hidinput_hid_event)
#define hidinput_report_event		LINUX_BACKPORT(hidinput_report_event)
#define hidinput_connect		LINUX_BACKPORT(hidinput_connect)
#define hidinput_disconnect		LINUX_BACKPORT(hidinput_disconnect)

#define hid_set_field			LINUX_BACKPORT(hid_set_field)
#define hid_input_report		LINUX_BACKPORT(hid_input_report)
#define hidinput_find_field		LINUX_BACKPORT(hidinput_find_field)
#define hidinput_get_led_field		LINUX_BACKPORT(hidinput_get_led_field)
#define hidinput_count_leds		LINUX_BACKPORT(hidinput_count_leds)
#define hidinput_calc_abs_res		LINUX_BACKPORT(hidinput_calc_abs_res)
#define hid_output_report		LINUX_BACKPORT(hid_output_report)
#define hid_allocate_device		LINUX_BACKPORT(hid_allocate_device)
#define hid_register_report		LINUX_BACKPORT(hid_register_report)
#define hid_parse_report		LINUX_BACKPORT(hid_parse_report)
#define hid_validate_values		LINUX_BACKPORT(hid_validate_values)
#define hid_open_report			LINUX_BACKPORT(hid_open_report)
#define hid_check_keys_pressed		LINUX_BACKPORT(hid_check_keys_pressed)
#define hid_connect			LINUX_BACKPORT(hid_connect)
#define hid_disconnect			LINUX_BACKPORT(hid_disconnect)
#define hid_match_id			LINUX_BACKPORT(hid_match_id)
#define hid_snto32			LINUX_BACKPORT(hid_snto32)


#define hid_report_raw_event		LINUX_BACKPORT(hid_report_raw_event)

#define usbhid_lookup_quirk		LINUX_BACKPORT(usbhid_lookup_quirk)
#define usbhid_quirks_init		LINUX_BACKPORT(usbhid_quirks_init)
#define usbhid_quirks_exit		LINUX_BACKPORT(usbhid_quirks_exit)
#define usbhid_set_leds			LINUX_BACKPORT(usbhid_set_leds)

#ifdef CONFIG_HID_PID
#define hid_pidff_init			LINUX_BACKPORT(hid_pidff_init)
#endif

/**
 * hid-debug functions
 */
#ifdef CONFIG_DEBUG_FS

#define hid_dump_input			LINUX_BACKPORT(hid_dump_input)
#define hid_dump_report			LINUX_BACKPORT(hid_dump_report)
#define hid_dump_device			LINUX_BACKPORT(hid_dump_device)
#define hid_dump_field			LINUX_BACKPORT(hid_dump_field)
#define hid_resolv_usage		LINUX_BACKPORT(hid_resolv_usage)
#define hid_debug_register		LINUX_BACKPORT(hid_debug_register)
#define hid_debug_unregister		LINUX_BACKPORT(hid_debug_unregister)
#define hid_debug_init			LINUX_BACKPORT(hid_debug_init)
#define hid_debug_exit			LINUX_BACKPORT(hid_debug_exit)
#define hid_debug_event			LINUX_BACKPORT(hid_debug_event)

#endif

/**
 * hiddev functions
 */

#ifdef CONFIG_USB_HIDDEV
#define hiddev_connect			LINUX_BACKPORT(hiddev_connect)
#define hiddev_disconnect		LINUX_BACKPORT(hiddev_disconnect)
#define hiddev_hid_event		LINUX_BACKPORT(hiddev_hid_event)
#define hiddev_report_event		LINUX_BACKPORT(hiddev_report_event)
#endif

/**
 * hidraw functions
 */

#ifdef CONFIG_HIDRAW
#define hidraw_init			LINUX_BACKPORT(hidraw_init)
#define hidraw_exit			LINUX_BACKPORT(hidraw_exit)
#define hidraw_report_event		LINUX_BACKPORT(hidraw_report_event)
#define hidraw_connect			LINUX_BACKPORT(hidraw_connect)
#define hidraw_disconnect		LINUX_BACKPORT(hidraw_disconnect)
#endif

#endif
