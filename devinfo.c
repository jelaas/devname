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

#include "libdevname.h"

#include "jelopt.h"

int main(int argc, char **argv)
{
	int err=0;
	struct dev_head result;
	struct devinfo_head sel;
	struct dev *dev;
	struct devinfo *info;
	struct devname *d;

	result.head = NULL;
	sel.head = NULL;
	
	if(jelopt(argv, 'h', "help", NULL, &err)) {
		printf("devinfo [-h]\n"
		       " version " LIBDEVNAME_VERSION "\n"
		       " Scans for and prints device information suitable as arguments for the\n"
		       " 'devname' program.\n");
		exit(0);
	}

	devname_usb_scan(&result, &sel);
	
	for(dev=result.head;dev;dev=dev->next) {
		int controller=0;
		
		/* filter out the system USB controllers */
		for(info=dev->info.head;info;info=info->next) {
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
		
		for(info=dev->info.head;info;info=info->next) {
			printf("%s=\"%s\" ", info->name, info->value);
		}
		for(d=dev->devnames.head;d;d=d->next) {
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
