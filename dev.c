/*
 * File: dev.c
 * Implements: main entry point for devicename scanning
 *
 * Copyright: Jens Låås, 2011
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */

#include "libdevname.h"

int dev_scan(struct jlhead *result, const struct jlhead *sel)
{
	return usb_scan(result,sel);
}
