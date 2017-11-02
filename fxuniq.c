#include <zlib.h>
#include <string.h>
#include <stdio.h>
#include "kseq.h"
KSEQ_INIT(gzFile, gzread)

#include "khash.h"
KHASH_SET_INIT_STR(str)

static void stk_printstr(const kstring_t *s, unsigned line_len)
{
	if (line_len != UINT_MAX && line_len != 0) {
		int i, rest = s->l;
		for (i = 0; i < s->l; i += line_len, rest -= line_len) {
			putchar('\n');
			if (rest > line_len) fwrite(s->s + i, 1, line_len, stdout);
			else fwrite(s->s + i, 1, rest, stdout);
		}
		putchar('\n');
	} else {
		putchar('\n');
		puts(s->s);
	}
}

void stk_printseq(const kseq_t *s, int line_len)
{
	putchar(s->qual.l? '@' : '>');
	fputs(s->name.s, stdout);
	if (s->comment.l) {
		putchar(' '); fputs(s->comment.s, stdout);
	}
	stk_printstr(&s->seq, line_len);
	if (s->qual.l) {
		putchar('+');
		stk_printstr(&s->qual, line_len);
	}
}

void process(const char * file, khash_t(str) *h) 
{
	gzFile fp;
	kseq_t *seq;
    int absent, l;
    khint_t k;
	fp = strcmp(file, "-")? gzopen(file, "r") : gzdopen(fileno(stdin), "r");
	seq = kseq_init(fp);
	while ((l = kseq_read(seq)) >= 0) {
        k = kh_put(str, h, seq->name.s, &absent);
        if (absent) 
        {
            stk_printseq(seq, 0);
            kh_key(h, k) = strdup(seq->name.s);
        } 
    }
    for (k = 0; k < kh_end(h); ++k)
    {
        if (kh_exist(h, k)) 
        {
            free((char*)kh_key(h, k));
        }
    }
	kseq_destroy(seq);
	gzclose(fp);
}

void help()
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage:   fxuniq [options] <in.fx>...\n");
    fprintf(stderr, "Options:\n\n");
    fprintf(stderr, "-%-10c%s\n", 'f', "ensure uniqueness within each file (allow duplicates beteen files)");
    fprintf(stderr, "-%-10c%s\n", 'h', "print this help message");
    fprintf(stderr, "\n\n");
}

int main(int argc, char * argv[])
{

    int c;
    int per_file = 0;
    while ((c = getopt (argc, argv, "fh")) != -1)
    {
        switch(c)
        {
            case 'f': per_file = 1; break;
            case 'h': help(); return 1; break;
        }
    }
    khash_t(str) *h = kh_init(str);
    if(optind >= argc)
    {
        process("-", h);
    }
    else 
    {
        int i;
        for(i = optind; i < argc; ++i)
        {
            process(argv[i], h);
            if(per_file) 
            {
                kh_clear(str, h);
            }
        }
    }
    kh_destroy(str, h);
	return 0;
}
