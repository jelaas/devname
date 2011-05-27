/*
 * File: dev.c
 * Implements: main entry point for devicename scanning
 *
 * Copyright: Jens Låås, 2011
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */

#include "libdevname.h"

int devname_dev_scan(struct dev_head *result, const struct devinfo_head *sel)
{
	return devname_usb_scan(result,sel);
}
