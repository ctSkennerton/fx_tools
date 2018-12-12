LDLIBS = -lz
PREFIX = /usr/bin/
progs := fxsummary fxuniq facount fqcount fxstats fxfilter

.PHONY: all install

all: $(progs)

install: $(progs)
	install $^ $(PREFIX)	

fxsummary: fxsummary.c 

fxuniq: fxuniq.c 

facount: facount.c

fqcount: fqcount.c

fxstats: fxstats.c 

fxfilter: fxfilter.c 
