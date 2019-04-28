CLIENTS := -DCLIENT_RAR5 -DCLIENT_ZIP
CFLAGS  := -Wall -Tdata=0 -O3 -mcrt=newlib -nostartfiles -D__USE_INLINE__ $(CLIENTS)
LIBS    := -larchive -lz -llzma -lbz2
SRCS    := extheader_os4.c common.c client.c
DEBUG   :=

.PHONY: all
all: libarchive

libarchive: $(SRCS)
	gcc $(CFLAGS) $(DEBUG) -o $@ $(SRCS) $(LIBS) 
#	strip $@
#	strip -R.comment $@
#	strip -R.comment `nm -u $@ | sed "s/U /-K/"` $@
