#ifndef DEVLOOKUP_H
#define DEVLOOKUP_H

#include <sys/types.h>

/*
 * Lookup 'devname' among the predefined device selections.
 * If a matching device is found return the name of this device in buf.
 *
 * If 'devname' is prefixed with "/dev/" then the result is also
 * prefixed with "/dev/".
 *
 * Returns number of devices found or a negative value if an error occured.
 */
int devname_lookup(char *buf, size_t bufsize, const char *devname);

/* also returns a constant name if supplied in the configuration */
int devname_lookup2(char *buf, size_t bufsize, char *constbuf, size_t constsize, const char *devname);

#endif
