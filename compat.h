#ifndef _COMPAT_H
#define _COMPAT_H

#include "compat-mt.h"

#ifndef HID_QUIRK_NO_EMPTY_INPUT
#define HID_QUIRK_NO_EMPTY_INPUT		0x00000100
#endif

#ifndef hidinput_calc_abs_res
/**
 * hidinput_calc_abs_res - calculate an absolute axis resolution
 * @field: the HID report field to calculate resolution for
 * @code: axis code
 *
 * The formula is:
 *                         (logical_maximum - logical_minimum)
 * resolution = ----------------------------------------------------------
 *              (physical_maximum - physical_minimum) * 10 ^ unit_exponent
 *
 * as seen in the HID specification v1.11 6.2.2.7 Global Items.
 *
 * Only exponent 1 length units are processed. Centimeters and inches are
 * converted to millimeters. Degrees are converted to radians.
 */
__s32 __compat_hidinput_calc_abs_res(const struct hid_field *field, __u16 code);
#define hidinput_calc_abs_res(a, b) __compat_hidinput_calc_abs_res((a), (b))
#endif


#ifndef hid_hw_request /* kernels >= 3.10 */
/**
 * hid_hw_request - send report request to device
 *
 * @hdev: hid device
 * @report: report to send
 * @reqtype: hid request type
 */
void __compat_hid_hw_request(struct hid_device *hdev,
				  struct hid_report *report, int reqtype);
#define hid_hw_request(a, b, c) __compat_hid_hw_request((a), (b), (c))
#endif /* hid_hw_request */

#ifndef hid_hw_idle /* kernels >= 3.10 */
/**
 * hid_hw_idle - send idle request to device
 *
 * @hdev: hid device
 * @report: report to control
 * @idle: idle state
 * @reqtype: hid request type
 */
int __compat_hid_hw_idle(struct hid_device *hdev, int report, int idle,
		int reqtype);
#define hid_hw_idle(a, b, c, d) __compat_hid_hw_idle((a), (b), (c), (d))
#endif /* hid_hw_idle */

#ifndef module_hid_driver
/**
 * module_hid_driver() - Helper macro for registering a HID driver
 * @__hid_driver: hid_driver struct
 *
 * Helper macro for HID drivers which do not do anything special in module
 * init/exit. This eliminates a lot of boilerplate. Each module may only
 * use this macro once, and calling it replaces module_init() and module_exit()
 */
#define module_hid_driver(__hid_driver) \
	module_driver(__hid_driver, hid_register_driver, \
		      hid_unregister_driver)
#endif /* module_hid_driver */

#endif
