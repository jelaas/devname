/*
 * File: devset.c
 * Implements: executes command in a mount namespace with a fixed named devicenode
 *
 * Copyright: Jens Låås, 2011
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */

#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mount.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <syscall.h>
#define unshare(flags) ((long)syscall(SYS_unshare, flags))

#include "libdevname.h"
#include "devlookup.h"
#include "jelopt.h"

int main(int argc, char **argv)
{
	int rc=0, err=0;
	char fn[256];
	char dev[512];
	char devname[512];
	mode_t mask;
	struct stat node;

	if(jelopt(argv, 'h', "help", NULL, &err)) {
	usage:
		printf("devset [-h] name cmd arg ..\n"
		       " version " LIBDEVNAME_VERSION "\n"
		       " Runs 'cmd' or bash in a separate mount name space.\n"
		       " This name space has its own mount of /dev with a devicenode created\n"
		       " according to configuration of the devname 'name'.\n"
			);
		exit(rc);
	}

	if(argc <= 1) {
		rc=1;
		goto usage;
	}

	if(devname_lookup2(dev, sizeof(dev), devname, sizeof(devname), argv[1])) {
		fprintf(stderr, "%s not found\n", argv[1]);
		return 1;
	}
	
	if(!devname[0]) {
		/* try heuristics to figure out a suitable devicenode name */

		/* FIXME: last digit -> 0. sd** -> sda1. sd* -> sda etc */
		/* alt let devname be the given devname alias... maybe not so bad? */
		strcpy(devname, argv[1]);
	}

	/* save info about dev/<dev> */
	sprintf(fn, "/dev/%s", dev);
	if(stat(fn, &node)) {
		fprintf(stderr, "Could not stat device node %s\n", fn);
		exit(1);
	}

	/* unshare mount namespace */
	if(unshare(CLONE_NEWNS)) {
		fprintf(stderr, "failed to unshare mount namespace\n");
		exit(1);
	}
	
	/* mount tmpfs on /dev */
	if(mount("tmpfs", "/dev", "tmpfs", 0, NULL)) {
		fprintf(stderr, "failed to mount tmpfs on /dev\n");
		exit(1);
	}
	
	/* create devicenode /dev/<devname> as a copy of /dev/<dev> */
	sprintf(fn, "/dev/%s", devname);
	mask = umask(0);
	if(mknod(fn, node.st_mode, node.st_rdev)) {
		fprintf(stderr, "Could not create device node %s\n", fn);
                exit(1);
	}
	umask(mask);
	
	chown(fn, node.st_uid, node.st_gid);
	
	/* exec argv[2] */

	argc--;
	argv++;
	argc--;
	argv++;
	if (!argc) {
		execl("/bin/sh", "-sh", NULL);
		exit(2);
	}

	execvp(argv[0], argv);
	exit(3);
}
