#include <zlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "kseq.h"

KSEQ_INIT(gzFile, gzread)

void help()
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage:   fxstats <in.fx>...\n\n");
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

int main(int argc, char * argv[])
{
	gzFile fp;
	kseq_t *seq;
    int print_file = 0;
    int print_header = 0;
    int c;
    while ((c = getopt (argc, argv, "hHf")) != -1)
    {
        switch(c)
        {
            case 'H': print_header = 1; break;
            case 'f': print_file = 1; break;
            case 'h': help(); return 1; break;
        }
    }

	if (optind + 1 > argc) {
        help();
		return 1;
	}
    if(argc - optind > 1)
    {
        // more than one input file, print the file name
        print_file = 1;
    }
    if( print_header) {
        if(print_file) {
            printf("file\t");
        }
        printf("name\tlength\tgc\tns\n");
    }
    int i;
    for(i = optind; i < argc; ++i)
    {
        fp = strcmp(argv[i], "-")? gzopen(argv[i], "r") : gzdopen(fileno(stdin), "r");
        seq = kseq_init(fp);
        

        int l;
        double gc_perc, ns_perc;
        while ((l = kseq_read(seq)) >= 0) {
            gc_perc = gc(seq->seq.s);
            ns_perc = ns(seq->seq.s);
            if(print_file)
            {
                printf("%s\t%s\t%d\t%.4f\t%.4f\n", argv[i], seq->name.s, l, gc_perc, ns_perc);
            } else {
                printf("%s\t%d\t%.4f\t%.4f\n", seq->name.s, l, gc_perc, ns_perc);
            }
        }

        gzclose(fp);
        kseq_destroy(seq);
    }
	return 0;
}
