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

int main(int argc, char * argv[])
{
	gzFile fp;
	kseq_t *seq;
	khash_t(str) *h = kh_init(str);
    int absent, l;
    khint_t k;

	if (optind + 1 > argc) {
		fprintf(stderr, "\n");
		fprintf(stderr, "Usage:   fxuniq <in.fa>\n\n");
		return 1;
	}
	fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r");
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
    kh_destroy(str, h);
	kseq_destroy(seq);
	gzclose(fp);
	return 0;
}
