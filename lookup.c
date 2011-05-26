/*
 * File: lookup.c
 * Implements: device name lookup functions
 *
 * Copyright: Jens Låås, 2011
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <fnmatch.h>
#include <stdlib.h>

#include "devlookup.h"
#include "libdevname.h"

#include "jelist.h"

/* /etc/devname.d/x.conf */

static int parse(const char *devname, char **class, char **devpattern, char **constdev, struct jlhead *sel)
{
	char *home, *pp, *p, *name, *value, fn[256], buf[512];
	int fd;
	ssize_t n;
	struct dev_info *di;
	
	*class = NULL;
	*devpattern = NULL;
	*constdev = NULL;
	
	sprintf(fn, "/etc/devname.d/%s.conf", devname);
	fd = open(fn, O_RDONLY);
	if(fd == -1) {
		home = getenv("HOME");
		if(home) {
			sprintf(fn, "%s/.devname.d/%s.conf", home, devname);
			fd = open(fn, O_RDONLY);
		}
	}
	if(fd == -1) return -1;
	
	n = read(fd, buf, sizeof(buf)-1);
	if(n <= 0)
		return -2;
	
	buf[n] = 0;

	p = buf;
	while(p && *p) {
		name = p;
		p = strchr(p, '\n');
		if(p) {
			*p = 0;
			p++;
		}
		value = strchr(name, '=');
		if(value) {
			*value = 0;
			value++;
			if(*value == '"') {
				value++;
				pp = strchr(value, '"');
				if(pp) *pp = 0;
			}
			
			if(!strcmp(name, "class")) {
				*class = strdup(value);
				continue;
			}
			if(!strcmp(name, "dev")) {
				*devpattern = strdup(value);
				continue;
			}
			if(!strcmp(name, "devname")) {
				*constdev = strdup(value);
				continue;
			}
			
			di = malloc(sizeof(struct dev_info));
			if(di) {
				di->name = strdup(name);
				di->value = strdup(value);
				jl_ins(sel, di);
			}
		}
	}

	close(fd);
	return 0;
}

static char *dev_match(struct dev *dev, char *devpattern)
{
	struct devname *devname;
	
	jl_foreach(dev->devnames, devname) {
		if(fnmatch(devpattern, devname->devname, 0)==0) {
			return devname->devname;
		}
	}
	return NULL;
}

int devname_lookup2(char *buf, size_t bufsize, char *constbuf, size_t constsize, const char *devname)
{
	struct jlhead *sel;
	struct jlhead *result;
	struct dev *dev;
	char *class, *devpattern, *constdev, *pfx="";
	struct devname *dn;
	
	if(strncmp(devname, "/dev/", 5)==0) {
		pfx = "/dev/";
		devname += 5;
	}

	sel = jl_new();
	
	/* parse config file in /etc/devname.d/ */
	if(parse(devname, &class, &devpattern, &constdev, sel))
		return -1;
	if(constbuf) {
		constbuf[0] = 0;
		if(constdev)
			strncpy(constbuf, constdev, constsize-1);
		constbuf[constsize-1] = 0;
	}
	
	result = jl_new();

	if(!class) class = "usb";

	/* look for usb devices */
	if(strcmp(class, "usb")==0)
		devname_usb_scan(result, sel);

	if(!devpattern) {
		dev = jl_head_first(result);
		if(dev) {
			dn = jl_head_first(dev->devnames);
			if(dn) {
				snprintf(buf, bufsize, "%s%s", pfx, dn->devname);
				return 0;
			}
		}
	}

	/* check for matching device node with devpattern among the devices that matched
	   selectors */
	jl_foreach(result, dev) {
		/* find any matching devicenode */
		if((devname = dev_match(dev, devpattern))) {
			snprintf(buf, bufsize, "%s%s", pfx, devname);
			return 0;
		}
	}
	return 1;
}

int devname_lookup(char *buf, size_t bufsize, const char *devname)
{
	return devname_lookup2(buf, bufsize, NULL, 0, devname);
}
