CC:=gcc
CFLAGS+=-Wall -Os -g
all:	devname-info devname-cfg devname-set libdevname.a devname-lookup
devname-info:	devinfo.o jelopt.o libdevname.a
	$(CC) $(LDFLAGS) -o devname-info devinfo.o jelopt.o libdevname.a
devname-cfg:	devname.o jelopt.o libdevname.a
	$(CC) $(LDFLAGS) -o devname-cfg devname.o jelopt.o libdevname.a
devname-lookup:	devlookup.o jelopt.o libdevname.a
	$(CC) $(LDFLAGS) -o devname-lookup devlookup.o jelopt.o libdevname.a
devname-set:		devset.o jelopt.o libdevname.a
	$(CC) $(LDFLAGS) -o devname-set devset.o jelopt.o libdevname.a
libdevname.a:	usb.o dev.o lookup.o
	ar cr libdevname.a usb.o dev.o lookup.o
clean:	
	rm -f *.o devname-info devname-cfg devname-lookup devname-set libdevname.a
install-lib:	libdevname.a
	mkdir -p $(DESTDIR)/usr/lib
	mkdir -p $(DESTDIR)/usr/include
	cp libdevname.a $(DESTDIR)/usr/lib
	cp devlookup.h $(DESTDIR)/usr/include
	cp libdevname.h $(DESTDIR)/usr/include
install-bin:	devname-info devname-cfg devname-set devname-lookup
	mkdir -p $(DESTDIR)/bin
	cp devname-lookup devname-cfg devname-set devname-info $(DESTDIR)/bin
install:	install-lib install-bin
