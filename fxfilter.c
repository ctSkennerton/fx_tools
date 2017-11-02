#include <zlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include "kseq.h"

KSEQ_INIT(gzFile, gzread)

void help()
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage:   fxfilter <in.fx>...\n\n");
}

int compare (const void * a, const void * b)
{
    return ( *(int*)a - *(int*)b );
}

double ns(const char * seq)
{
    size_t ns = 0;
    const char * ptr = seq;
    for(; *ptr; ptr++)
    {
        if(*ptr == 'n')
        {
            ns++;
        }
    }
    return (double)ns / (ptr - seq);
}

double gc(const char * seq)
{
    size_t gc = 0;
    const char * ptr = seq;
    for(; *ptr; ptr++)
    {
        char mask = *ptr & 'G' & 'C';
        if(mask == ('G' & 'C'))
        {
            gc++;
        }
    }
    return (double)gc / (ptr - seq);
}

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

void process(const char * file, int lower_length, int upper_length, double lower_gc, double upper_gc)
{
	gzFile fp;
	kseq_t *seq;
    fp = strcmp(file, "-") ? gzopen(file, "r") : gzdopen(fileno(stdin), "r");
    seq = kseq_init(fp);
    

    int l;
    double gc_perc, ns_perc;
    while ((l = kseq_read(seq)) >= 0) {
        gc_perc = gc(seq->seq.s);
        //ns_perc = ns(seq->seq.s);
        if(l >= lower_length && l <= upper_length)
        {
            if(gc_perc >= lower_gc && gc_perc <= upper_gc)
            {
                stk_printseq(seq, 0);
            }
        }
    }

    gzclose(fp);
    kseq_destroy(seq);
}

int main(int argc, char * argv[])
{
    int lower_length = 0;
    int upper_length = INT_MAX;
    double lower_gc = 0.0;
    double upper_gc = 1.0;
    int c;
    while ((c = getopt (argc, argv, "hl:L:g:G:n:N:f")) != -1)
    {
        switch(c)
        {
            case 'l': lower_length = atoi(optarg); break;
            case 'L': upper_length = atoi(optarg); break;
            case 'g': lower_gc = atof(optarg); break;
            case 'G': upper_gc = atof(optarg); break;
            case 'h': help(); return 1; break;
        }
    }

	if (optind >= argc)
    {
        process("-", lower_length, upper_length, lower_gc, upper_gc);
	}
    else 
    {
        int i;
        for(i = optind; i < argc; ++i)
        {
            process(argv[i], lower_length, upper_length, lower_gc, upper_gc);
        }
    }
	return 0;
}
