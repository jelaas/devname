/*
 * File: usb.c
 * Implements: scanning of usb devices
 *
 * Copyright: Jens Låås, 2011
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <ctype.h>
#include <fnmatch.h>

#include "jelist.h"

#include "libdevname.h"

static int usb_scan_dir(struct jlhead *result, const struct jlhead *sel, const char *dir);

static char *getstring(const char *dir, const char *file)
{
	char fn[512];
	char buf[256];
	int fd, n;
	
	sprintf(fn, "%s/%s", dir, file);
	fd = open(fn, O_RDONLY);
	if(fd == -1) return NULL;

	n = read(fd, buf, sizeof(buf)-1);
	if(n >= 0) {
		buf[n] = 0;
		if(n > 0) {
			if(buf[n-1] == '\n')
				buf[n-1] = 0;
		}
	} else buf[0] = 0;
	buf[sizeof(buf)-1] = 0;
	close(fd);
	return strdup(buf);
}

/*
 * scan usb devices.
 * add matching devices to result list
 */
int devname_usb_scan(struct jlhead *result, const struct jlhead *sel)
{
	return usb_scan_dir(result, sel, "/sys/bus/usb/devices");
}

static char *dev_probe(const char *name)
{
	char fn[512];
	struct stat statb;

	sprintf(fn, "/dev/%s", name);
	if(stat(fn, &statb)==0) {
		if(S_ISBLK(statb.st_mode))
			return "b";
		if(S_ISCHR(statb.st_mode))
			return "c";
	}
	return "f";
}

static struct devname *devname_new(const char *dir)
{
	struct devname *dn;
	char *name;

	name = basename(strdup(dir));
	if(isdigit(name[0]) )
		return NULL;
	
	dn = malloc(sizeof(struct devname));
	dn->dev = getstring(dir, "dev");
	dn->devname = name;
	dn->type = dev_probe(name);
	dn->pos = strlen(dir);
//	printf("devname_new %s %s %s\n", dn->dev, dn->devname, dir);
	return dn;
}

/*
 * depth-first scan for all dev files. name of parentdir is assumed to be node name.
 */
static int usb_scan_devname(const char *dir, struct jlhead *devnames)
{
	DIR *d;
	struct dirent *ent;
	char fn[512];
	
//	printf("scan_devname opening dir %s\n", dir);
	d = opendir(dir);
	if(!d) return 0;
	
	while((ent = readdir(d))) {
		if(ent->d_type == DT_LNK) continue;
		if(ent->d_type == DT_UNKNOWN) {
			/* FIXME: use lstat to determine if file is a link */
			printf("DT_UNKNOWN\n");
			exit(1);
		}
		if(ent->d_name[0] != '.') {
			sprintf(fn, "%s/%s", dir, ent->d_name);
			usb_scan_devname(fn, devnames);
		}
		if(!strcmp(ent->d_name, "dev")) {
			struct devname *devname;
			devname = devname_new(dir);
			if(devname)
				jl_ins(devnames, devname);
		}
	}
	closedir(d);
	return devnames->len;
}

static int dev_info_add(struct dev *dev, const char *dir, const char *name)
{
	struct dev_info *info;
	char *value;
	
	value = getstring(dir, name);

	if(value) {
		info = malloc(sizeof(struct dev_info));
		info->name = name;
		info->value = value;
	
		jl_ins(dev->info, info);
		return 0;
	}
	return -1;
}

static struct dev *dev_new()
{
	struct dev *d;
	d = malloc(sizeof(struct dev));
	d->class = "usb";
	return d;
}
static int dev_match(const struct jlhead *info, const struct jlhead *selectors)
{
	int match=1;
	struct dev_info *sel, *i;
	
	jl_foreach(selectors, sel) {
		match=0;
		jl_foreach(info, i) {
			if(strcmp(i->name, sel->name)==0) {
				if(fnmatch(sel->value, i->value, 0)==0) {
					match=1;
					break;
				}
			}
		}
		if(match==0)
			return 0;
	}
	return match;
}

static int usb_scan_dev(struct jlhead *result, const struct jlhead *sel, const char *dir)
{
	char *usbdev;
	struct jlhead *devnames;
	struct dev *dev;
	
	usbdev = getstring(dir, "dev");
	if(usbdev) {
		devnames = jl_new();
		
		if(usb_scan_devname(dir, devnames)) {
			dev = dev_new();
			dev->devnames = devnames;
			dev->info = jl_new();
			dev_info_add(dev, dir, "serial");
			dev_info_add(dev, dir, "manufacturer");
			dev_info_add(dev, dir, "product");
			dev_info_add(dev, dir, "idProduct");
			dev_info_add(dev, dir, "idVendor");

			if(dev_match(dev->info, sel))
				jl_ins(result, dev);
		}
	}
	return 0;
}

static int usb_scan_dir(struct jlhead *result, const struct jlhead *sel, const char *dir)
{
	DIR *d;
	struct dirent *ent;
	char fn[512];
	
/*
 /sys/bus/usb/devices/usb1/1-4/serial
 /sys/bus/usb/devices/1-4:1.0/serial

 */
//	printf("opening dir %s\n", dir);
	d = opendir(dir);
	if(!d) return 0;

	while((ent = readdir(d))) {
		if(ent->d_name[0] != '.') {
			sprintf(fn, "%s/%s", dir, ent->d_name);
			usb_scan_dev(result, sel, fn);
		}
	}
	closedir(d);
	return 0;
}
