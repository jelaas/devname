/*
 * File: devlookup.c
 * Implements: device name lookup via command
 *
 * Copyright: Jens Låås, 2011
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "libdevname.h"
#include "devlookup.h"
#include "jelopt.h"

int main(int argc, char **argv)
{
	int err=0, rc=0;
	char buf[512];

	if(jelopt(argv, 'h', "help", NULL, &err)) {
	usage:
		printf("devlookup [-h] name\n"
		       " version " LIBDEVNAME_VERSION "\n"
		       " Lookup devicenode for devname 'name'. Print result on stdout.\n"
			);
		exit(rc);
	}

	argc = jelopt_final(argv, &err);

	if(argc <= 1) {
		rc=1;
		goto usage;
	}

	if(devname_lookup(buf, sizeof(buf), argv[1])==0)
		printf("%s\n", buf);
	else {
		fprintf(stderr, "%s not found\n", argv[1]);
		exit(1);
	}
	exit(0);
}
