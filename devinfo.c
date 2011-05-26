/*
 * File: devinfo.c
 * Implements: displaying deviceinformation in format suitable for devname cmd
 *
 * Copyright: Jens Låås, 2011
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "jelist.h"
#include "libdevname.h"

int main()
{
	struct jlhead *result;
	struct jlhead *sel;
	struct dev *dev;
	struct dev_info *info;
	struct devname *d;

	result = jl_new();
	sel = jl_new();

	usb_scan(result, sel);
	
	jl_foreach(result, dev) {
		int controller=0;
		
		/* filter out the system USB controllers */
		jl_foreach(dev->info, info) {
			if(strcmp(info->name, "idVendor")==0) {
				if(strcmp(info->value, "1d6b")==0)
					controller++;
			}
			if(strcmp(info->name, "idProduct")==0) {
				if(strncmp(info->value, "000", 3)==0)
					controller++;
			}
		}
		if(controller==2) continue;
		
		jl_foreach(dev->info, info) {
			printf("%s=\"%s\" ", info->name, info->value);
		}
		jl_foreach(dev->devnames, d) {
			char fn[256];
			struct stat statb;
			sprintf(fn, "/dev/%s", d->devname);
			if(stat(fn, &statb)==0) {
				printf("dev=%s ", d->devname);
				continue;
			}
			sprintf(fn, "/dev/input/%s", d->devname);
			if(stat(fn, &statb)==0) {
				printf("dev=%s ", d->devname);
				continue;
			}
		}
		printf("\n");
	}
	exit(0);
}
