/*
 * File: devlookup.c
 * Implements: device name lookup via command
 *
 * Copyright: Jens Låås, 2011
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */

#include <stdio.h>

#include "devlookup.h"

#include "jelist.h"

int main(int argc, char **argv)
{
	char buf[512];

	if(devname_lookup(buf, sizeof(buf), argv[1])==0)
		printf("%s\n", buf);
	else {
		fprintf(stderr, "not found\n");
		return 1;
	}
	return 0;
}
