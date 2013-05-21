#ifndef _COMPAT_H
#define _COMPAT_H

#include "compat-mt.h"
#include <linux/hid.h>

#ifndef HID_GROUP_MULTITOUCH
#define HID_GROUP_MULTITOUCH 0
#undef HID_DEVICE
#undef HID_USB_DEVICE
#undef HID_BLUETOOTH_DEVICE
#define HID_DEVICE(b, g, ven, prod)					\
	.bus = (b), .vendor = (ven), .product = (prod)
#define HID_USB_DEVICE(ven, prod)				\
	.bus = BUS_USB, .vendor = (ven), .product = (prod)
#define HID_BLUETOOTH_DEVICE(ven, prod)					\
	.bus = BUS_BLUETOOTH, .vendor = (ven), .product = (prod)
#endif /* HID_GROUP_MULTITOUCH */

/**
 * struct __compat_hid_driver
 * @name: driver name (e.g. "Footech_bar-wheel")
 * @id_table: which devices is this driver for (must be non-NULL for probe
 * 	      to be called)
 * @probe: new device inserted
 * @remove: device removed (NULL if not a hot-plug capable driver)
 * @report_table: on which reports to call raw_event (NULL means all)
 * @raw_event: if report in report_table, this hook is called (NULL means nop)
 * @usage_table: on which events to call event (NULL means all)
 * @event: if usage in usage_table, this hook is called (NULL means nop)
 * @report: this hook is called after parsing a report (NULL means nop)
 * @report_fixup: called before report descriptor parsing (NULL means nop)
 * @input_mapping: invoked on input registering before mapping an usage
 * @input_mapped: invoked on input registering after mapping an usage
 * @input_configured: invoked just before the device is registered
 * @feature_mapping: invoked on feature registering
 * @suspend: invoked on suspend (NULL means nop)
 * @resume: invoked on resume if device was not reset (NULL means nop)
 * @reset_resume: invoked on resume if device was reset (NULL means nop)
 *
 * probe should return -errno on error, or 0 on success. During probe,
 * input will not be passed to raw_event unless hid_device_io_start is
 * called.
 *
 * raw_event and event should return 0 on no action performed, 1 when no
 * further processing should be done and negative on error
 *
 * input_mapping shall return a negative value to completely ignore this usage
 * (e.g. doubled or invalid usage), zero to continue with parsing of this
 * usage by generic code (no special handling needed) or positive to skip
 * generic parsing (needed special handling which was done in the hook already)
 * input_mapped shall return negative to inform the layer that this usage
 * should not be considered for further processing or zero to notify that
 * no processing was performed and should be done in a generic manner
 * Both these functions may be NULL which means the same behavior as returning
 * zero from them.
 */
struct __compat_hid_driver {
	char *name;
	const struct hid_device_id *id_table;

	int (*probe)(struct hid_device *dev, const struct hid_device_id *id);
	void (*remove)(struct hid_device *dev);

	const struct hid_report_id *report_table;
	int (*raw_event)(struct hid_device *hdev, struct hid_report *report,
			u8 *data, int size);
	const struct hid_usage_id *usage_table;
	int (*event)(struct hid_device *hdev, struct hid_field *field,
			struct hid_usage *usage, __s32 value);
	void (*report)(struct hid_device *hdev, struct hid_report *report);

	__u8 *(*report_fixup)(struct hid_device *hdev, __u8 *buf,
			unsigned int *size);

	int (*input_mapping)(struct hid_device *hdev,
			struct hid_input *hidinput, struct hid_field *field,
			struct hid_usage *usage, unsigned long **bit, int *max,
			unsigned usage_index);
	int (*input_mapped)(struct hid_device *hdev,
			struct hid_input *hidinput, struct hid_field *field,
			struct hid_usage *usage, unsigned long **bit, int *max);
	void (*input_configured)(struct hid_device *hdev,
				 struct hid_input *hidinput);
	void (*feature_mapping)(struct hid_device *hdev,
			struct hid_field *field,
			struct hid_usage *usage);
#ifdef CONFIG_PM
	int (*suspend)(struct hid_device *hdev, pm_message_t message);
	int (*resume)(struct hid_device *hdev);
	int (*reset_resume)(struct hid_device *hdev);
#endif
	/* private */
	struct hid_driver hdrv;
};

int __compat___hid_register_driver(struct __compat_hid_driver *hdrv,
		struct module *owner, const char *mod_name);

/* use a define to avoid include chaining to get THIS_MODULE & friends */
#define __compat_hid_register_driver(driver) \
	__compat___hid_register_driver(driver, THIS_MODULE, KBUILD_MODNAME)

void __compat_hid_unregister_driver(struct __compat_hid_driver *hdrv);


#ifdef module_hid_driver
#undef module_hid_driver
#endif /* module_hid_driver */
/**
 * module_hid_driver() - Helper macro for registering a HID driver
 * @__hid_driver: hid_driver struct
 *
 * Helper macro for HID drivers which do not do anything special in module
 * init/exit. This eliminates a lot of boilerplate. Each module may only
 * use this macro once, and calling it replaces module_init() and module_exit()
 */
#define module_hid_driver(__hid_driver) \
	module_driver(__hid_driver, __compat_hid_register_driver, \
		      __compat_hid_unregister_driver)

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

#endif
