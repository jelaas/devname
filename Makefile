CC:=gcc
CFLAGS+=-Wall -Os -g
all:	devinfo devname devset libdevname.a devlookup
devinfo:	devinfo.o jelist.o libdevname.a
devname:	devname.o jelist.o jelopt.o libdevname.a
devlookup:	devlookup.o jelist.o jelopt.o libdevname.a
devset:		devset.o jelist.o jelopt.o libdevname.a
libdevname.a:	usb.o dev.o lookup.o
	ar cr libdevname.a usb.o dev.o lookup.o
clean:	
	rm -f *.o devinfo devname devlookup devset libdevname.a
install-lib:	libdevname.a
	mkdir -p $(DESTDIR)/usr/lib
	mkdir -p $(DESTDIR)/usr/include
	cp libdevname.a $(DESTDIR)/usr/lib
	cp devlookup.h $(DESTDIR)/usr/include
	cp libdevname.h $(DESTDIR)/usr/include
install-bin:	devinfo devname devset devlookup
	mkdir -p $(DESTDIR)/bin
	cp devlookup devname devset devinfo $(DESTDIR)/bin
install:	install-lib install-bin
