MAJOR	= 1
MINOR	= 0

CC	= gcc -std=c99
CFLAGS	= -Wall -Wstrict-prototypes -O2
LDFLAGS	= -fPIC -shared
# -Wl,-soname,libsbv.so.$(MAJOR)
TESTS	= encode-test

libsbv.so.$(MAJOR): libsbv.so.$(MAJOR).$(MINOR)
	ln -s $< $@
	
libsbv.so.1.0: sbv.c cache.c encode.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(TESTS) libsbv.so.*

test:	$(TESTS)
	for i in $(TESTS); do ./$$i; done

encode-test: encode-test.c encode.c
	$(CC) $(CFLAGS) -o $@ $^