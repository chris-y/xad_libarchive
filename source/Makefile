CLIENTS := -DCLIENT_RAR5
EXTRA_CLIENTS := -DCLIENT_ZIP
DEBUG   :=
#-gstabs
CFLAGS  := -Wall -O3 -D__USE_INLINE__ $(DEBUG) $(CLIENTS) #$(EXTRA_CLIENTS)
LDFLAGS := -Tdata=0 -nostartfiles
LIBS    := -larchive -lz -llzma -lbz2
OBJS    := extheader_os4.o common.o client.o

.PHONY: all
all: libarchive

$(OBJS): $(OBJS:.o=.c)

libarchive: $(OBJS)
	gcc $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
#	strip $@
#	strip -R.comment $@
#	strip -R.comment `nm -u $@ | sed "s/U /-K/"` $@
