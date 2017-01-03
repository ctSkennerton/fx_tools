LDLIBS = -lz
PREFIX = /usr/bin/
progs := fxsummary fxuniq facount fqcount fxstats

.PHONY: all install

all: $(progs)

install: $(progs)
	install -t $(PREFIX) $^	

fxsummary: fxsummary.c kvec.h kseq.h

fxuniq: fxuniq.c kseq.h khash.h

facount: facount.c

fqcount: fqcount.c

fxstats: fxstats.c kseq.h 
