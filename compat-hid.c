#include <linux/device.h>
#include "compat-hid.h"
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/usb.h>
#include "usbhid/usbhid.h"

static s32 __compat_snto32(__u32 value, unsigned n)
{
	switch (n) {
	case 8:  return ((__s8)value);
	case 16: return ((__s16)value);
	case 32: return ((__s32)value);
	}
	return value & (1 << (n - 1)) ? value | (-1 << n) : value;
}

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
__s32 __compat_hidinput_calc_abs_res(const struct hid_field *field, __u16 code)
{
	__s32 unit_exponent = field->unit_exponent;
	__s32 logical_extents = field->logical_maximum -
					field->logical_minimum;
	__s32 physical_extents = field->physical_maximum -
					field->physical_minimum;
	__s32 prev;

	/* Check if the extents are sane */
	if (logical_extents <= 0 || physical_extents <= 0)
		return 0;

	/*
	 * Verify and convert units.
	 * See HID specification v1.11 6.2.2.7 Global Items for unit decoding
	 */
	switch (code) {
	case ABS_X:
	case ABS_Y:
	case ABS_Z:
	case ABS_MT_POSITION_X:
	case ABS_MT_POSITION_Y:
	case ABS_MT_TOOL_X:
	case ABS_MT_TOOL_Y:
	case ABS_MT_TOUCH_MAJOR:
	case ABS_MT_TOUCH_MINOR:
		if (field->unit & 0xffffff00)		/* Not a length */
			return 0;
		unit_exponent += __compat_snto32(field->unit >> 4, 4) - 1;
		switch (field->unit & 0xf) {
		case 0x1:				/* If centimeters */
			/* Convert to millimeters */
			unit_exponent += 1;
			break;
		case 0x3:				/* If inches */
			/* Convert to millimeters */
			prev = physical_extents;
			physical_extents *= 254;
			if (physical_extents < prev)
				return 0;
			unit_exponent -= 1;
			break;
		default:
			return 0;
		}
		break;

	case ABS_RX:
	case ABS_RY:
	case ABS_RZ:
	case ABS_TILT_X:
	case ABS_TILT_Y:
		if (field->unit == 0x14) {		/* If degrees */
			/* Convert to radians */
			prev = logical_extents;
			logical_extents *= 573;
			if (logical_extents < prev)
				return 0;
			unit_exponent += 1;
		} else if (field->unit != 0x12) {	/* If not radians */
			return 0;
		}
		break;

	default:
		return 0;
	}

	/* Apply negative unit exponent */
	for (; unit_exponent < 0; unit_exponent++) {
		prev = logical_extents;
		logical_extents *= 10;
		if (logical_extents < prev)
			return 0;
	}
	/* Apply positive unit exponent */
	for (; unit_exponent > 0; unit_exponent--) {
		prev = physical_extents;
		physical_extents *= 10;
		if (physical_extents < prev)
			return 0;
	}

	/* Calculate resolution */
	return DIV_ROUND_CLOSEST(logical_extents, physical_extents);
}
EXPORT_SYMBOL_GPL(__compat_hidinput_calc_abs_res);


/**
 * hid_hw_request - send report request to device
 *
 * @hdev: hid device
 * @report: report to send
 * @reqtype: hid request type
 */
void __compat_hid_hw_request(struct hid_device *hdev,
				  struct hid_report *report, int reqtype)
{
	if (hdev->bus != BUS_USB)
		return;

	return;

	switch (reqtype) {
	case HID_REQ_GET_REPORT:
		usbhid_submit_report(hdev, report, USB_DIR_IN);
		break;
	case HID_REQ_SET_REPORT:
		usbhid_submit_report(hdev, report, USB_DIR_OUT);
		break;
	}
}
EXPORT_SYMBOL_GPL(__compat_hid_hw_request);

static int hid_set_idle(struct usb_device *dev, int ifnum, int report, int idle)
{
	return usb_control_msg(dev, usb_sndctrlpipe(dev, 0),
		HID_REQ_SET_IDLE, USB_TYPE_CLASS | USB_RECIP_INTERFACE, (idle << 8) | report,
		ifnum, NULL, 0, USB_CTRL_SET_TIMEOUT);
}

/**
 * hid_hw_idle - send idle request to device
 *
 * @hdev: hid device
 * @report: report to control
 * @idle: idle state
 * @reqtype: hid request type
 */
int __compat_hid_hw_idle(struct hid_device *hdev, int report, int idle,
		int reqtype)
{
	struct usb_device *dev;
	struct usb_interface *intf;
	struct usb_host_interface *interface;
	int ifnum;

	if (hdev->bus != BUS_USB)
		return 0;

	return 0;

	dev = hid_to_usb_dev(hdev);
	intf = to_usb_interface(hdev->dev.parent);
	interface = intf->cur_altsetting;
	ifnum = interface->desc.bInterfaceNumber;

	if (reqtype != HID_REQ_SET_IDLE)
		return -EINVAL;

	hid_set_idle(dev, ifnum, report, idle);

	return 0;
}
EXPORT_SYMBOL_GPL(__compat_hid_hw_idle);

static unsigned get_usage_index(struct hid_field *field,
				struct hid_usage *usage)
{
	return usage - field->usage;
}

static int end_of_report(struct hid_field *field, struct hid_usage *usage)
{
	struct hid_report *report = field->report;
	unsigned next_field_index = field->index + 1;
	unsigned usage_index = get_usage_index(field, usage);

	if (usage_index + 1 != field->report_count)
		/* we are not on the last usage of the incoming field. */
		return 0;

	return next_field_index == report->maxfield;
}

static int last_variable_field(struct hid_field *field, struct hid_usage *usage)
{
	struct hid_report *report = field->report;
	unsigned next_field_index = field->index + 1;
	unsigned usage_index = get_usage_index(field, usage);

	if (usage_index + 1 != field->report_count)
		/* we are not on the last usage of the incoming field. */
		return 0;

	while (next_field_index < report->maxfield) {
		struct hid_field *next_field = report->field[next_field_index];
		if (!(next_field->flags & HID_MAIN_ITEM_CONSTANT))
			/*
			 * the next field contains data: we are not at the end
			 * of the report.
			 */
			return 0;
		next_field_index++;
	}

	/*
	 * all the fields after the given one are constants, meaning that
	 * no other processing will be done on them.
	 * We are at the end of the report.
	 */
	return 1;
}

static int __compat_input_mapping(struct hid_device *hdev, struct hid_input *hi,
		struct hid_field *field, struct hid_usage *usage,
		unsigned long **bit, int *max)
{
	struct hid_driver *hdrv = hdev->driver;
	struct __compat_hid_driver *c_hdrv = container_of(hdrv, struct __compat_hid_driver, hdrv);
	unsigned usage_index = get_usage_index(field, usage);
	int ret = 0;

	if (c_hdrv->input_mapping)
		ret = c_hdrv->input_mapping(hdev, hi, field, usage, bit, max,
					    usage_index);

	if (c_hdrv->input_configured && last_variable_field(field, usage)) {
		hi->report = field->report;
		c_hdrv->input_configured(hdev, hi);
	}

	return ret;
}

static int __compat_event(struct hid_device *hid, struct hid_field *field,
				struct hid_usage *usage, __s32 value)
{
	struct hid_driver *hdrv = hid->driver;
	struct __compat_hid_driver *c_hdrv = container_of(hdrv, struct __compat_hid_driver, hdrv);
	unsigned usage_index = get_usage_index(field, usage);
	int ret = 0;

	if (c_hdrv->event)
		ret = c_hdrv->event(hid, field, usage, value);

	field->value[usage_index] = value;

	if (c_hdrv->report && end_of_report(field, usage))
		/* we are on the last field of the incoming report. */
		c_hdrv->report(hid, field->report);

	return ret;
}

int __compat___hid_register_driver(struct __compat_hid_driver *__hid_driver,
		struct module *owner, const char *mod_name)
{
	struct hid_driver *hdrv = &__hid_driver->hdrv;
	hdrv->name = __hid_driver->name;
	hdrv->id_table = __hid_driver->id_table;

	hdrv->probe = __hid_driver->probe;
	hdrv->remove = __hid_driver->remove;

	hdrv->report_table = __hid_driver->report_table;
	hdrv->raw_event = __hid_driver->raw_event;
	hdrv->usage_table = __hid_driver->usage_table;
	hdrv->event = &__compat_event;

	hdrv->report_fixup = __hid_driver->report_fixup;

	hdrv->input_mapping = &__compat_input_mapping;
	hdrv->input_mapped = __hid_driver->input_mapped;
	hdrv->feature_mapping = __hid_driver->feature_mapping;
#ifdef CONFIG_PM
	hdrv->suspend = __hid_driver->suspend;
	hdrv->resume = __hid_driver->resume;
	hdrv->reset_resume = __hid_driver->reset_resume;
#endif
	return __hid_register_driver(hdrv, owner, mod_name);
}
EXPORT_SYMBOL_GPL(__compat___hid_register_driver);

void __compat_hid_unregister_driver(struct __compat_hid_driver *hdrv)
{
	hid_unregister_driver(&hdrv->hdrv);
}
EXPORT_SYMBOL_GPL(__compat_hid_unregister_driver);