#ifndef LIBDEVNAME_H
#define LIBDEVNAME_H

#include "jelist.h"

/*
 * Find devices matching selections in list 'sel' (of type struct dev_info).
 * Put matching devices in result list 'result'.
 */
int devname_dev_scan(struct jlhead *result, const struct jlhead *sel);

/*
 * Find USB devices matching selections in list 'sel' (of type struct dev_info).
 * Put matching USB devices in result list 'result'.
 */
int devname_usb_scan(struct jlhead *result, const struct jlhead *sel);

struct devname {
  char *dev;
  char *devname;
  char *type;
  int pos;
};

struct dev_info {
  const char *name;
  const char *value;
};

struct dev {
  struct jlhead *info; /* list of struct dev_info */
  struct jlhead *devnames; /* list of struct devname */
  char *class; /* "usb", "pci" etc */
};

#endif
