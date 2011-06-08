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
#include <dirent.h>

#include "libdevname.h"
#include "devlookup.h"

#include "jelopt.h"

static int strsuffix(const char *s, const char *suf)
{
	if(strlen(suf) > strlen(s))
		return 0;
	s+=(strlen(s)-strlen(suf));
	return strcmp(s, suf)==0;
}

static void conf_lookup(const char *pfx, DIR *dir)
{
	struct dirent *ent;
	char *p, name[256], node[256];
	while( (ent=readdir(dir)) ) {
		if(strsuffix(ent->d_name, ".conf")) {
			strncpy(name, ent->d_name, sizeof(name));
			p = strrchr(name, '.');
			if(p) *p = 0;
			if(devname_lookup(node, sizeof(node), name)>0)
				printf("%s: %s -> %s\n", pfx, name, node);
			else
				printf("%s: %s not found\n", pfx, name);
		}
	}	
}

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
		printf("devname-info [-h]\n"
		       " version " LIBDEVNAME_VERSION "\n"
		       " -l --list -- list devnames and lookup info\n"
		       " Scans for and prints device information suitable as arguments for the\n"
		       " 'devname' program.\n");
		exit(0);
	}

	if(jelopt(argv, 'l', "list", NULL, &err)) {
		DIR *dir;
		char *home, fn[256];
		
		strcpy(fn, "/etc/devname.d");
		dir = opendir(fn);
		if(dir) {
			conf_lookup("sys", dir);
			closedir(dir);
		}
		home = getenv("HOME");
		if(home) {
			snprintf(fn, sizeof(fn), "%s/.devname.d", home);
			dir = opendir(fn);
			if(dir) {
				conf_lookup("user", dir);
				closedir(dir);
			}
		}

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
