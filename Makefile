LDLIBS = -lz

progs := fxsummary fxuniq facount fqcount fxstats

all: $(progs)

fxsummary: fxsummary.c kvec.h kseq.h

fxuniq: fxuniq.c kseq.h khash.h

facount: facount.c

fqcount: fqcount.c

fxstats: fxstats.c kseq.h 
