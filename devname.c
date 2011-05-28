/*
 * File: devname.c
 * Implements: device name configuration
 *
 * Copyright: Jens L��s, 2011
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */

/*

devname name dev=DEVPATTERN name=value

DEV matches devicename with fnmatch()

heuristics:

NNN[0-9] is transformed to NNN.

class can be inferred from selectors used ?

/etc/devname.d/x.conf

x:
class=usb
dev=DEVPATTERN
devname=DEVCONSTNAME
sel=val
..

 */

/*
 devname --list # list all devnames

 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "jelopt.h"

#include "libdevname.h"
#include "jelopt.h"

int main(int argc, char **argv)
{
	int fd, i, err=0;
	char *dev;
	mode_t mask;
	char fn[256];

	if(jelopt(argv, 'h', "help", NULL, &err)) {
		printf("devname [-h] name selector ..\n"
		       " version " LIBDEVNAME_VERSION "\n"
		       " 'name' is your handle for the device that matches the given selectors.\n"
		       " 'selector' is a 'name=value' pair. Suitable selectors is reported by the\n"
		       " devinfo program.\n"
			);
		exit(0);
	}
	
	argc = jelopt_final(argv, &err);
	
	dev=argv[1];
	if(getuid()==0) {
		mask = umask(0);
		sprintf(fn, "/etc/devname.d/%s.conf", dev);
		mkdir("/etc/devname.d", 1755);
		fd = open(fn, O_RDWR|O_CREAT|O_EXCL, 0644);
		umask(mask);
	} else {
		sprintf(fn, "%s/.devname.d", getenv("HOME"));
		mkdir(fn, 1755);
		sprintf(fn, "%s/.devname.d/%s.conf", getenv("HOME"), dev);
		fd = open(fn, O_RDWR|O_CREAT|O_EXCL, 0644);
	}
	if(fd == -1) {
		printf("Cannot create %s (must not already exist!)\n", fn);
		exit(1);
	}
	argc--;
	argv++;
	
	/* build selector list */
	for(i=1;i<argc;i++) {
		if(strchr(argv[i], '=')) {
			write(fd, argv[i], strlen(argv[i]));
			write(fd, "\n", 1);
		}
	}
	close(fd);

	return 0;
}
