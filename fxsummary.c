#include <zlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "kseq.h"
#include "kvec.h"

KSEQ_INIT(gzFile, gzread)

void help()
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage:   fxsummary [options] <in.fx>...\n");
    fprintf(stderr, "Options:\n\n");
    fprintf(stderr, "-%-10c%s\n", 'H', "print a header line at the beginning of the output");
    fprintf(stderr, "-%-10c%s\n", 'f', "print the file name at the beggining of each output line");
    fprintf(stderr, "-%-10c%s\n", 'h', "print this help message");
    fprintf(stderr, "\n\n");
}

int compare (const void * a, const void * b)
{
    return ( *(int*)a - *(int*)b );
}

void process(const char * file, int printFile) 
{
	gzFile fp;
	kseq_t *seq;
    fp = strcmp(file, "-")? gzopen(file, "r") : gzdopen(fileno(stdin), "r");
    seq = kseq_init(fp);
    
    kvec_t(int) array;
    kv_init(array);

    int l;
    unsigned long sum, count;
    sum = count = 0;
    while ((l = kseq_read(seq)) >= 0) {
       sum += l;
       count += 1;
       kv_push(int, array, l);
    }

    qsort(array.a, kv_size(array), sizeof(int), compare);
    int j, k, n50, mean, median, min, max;
    k = j = 0;
    n50 = 0;
    int n50_length = sum / 2;
    while(j < kv_size(array)) {
        k += kv_A(array, j);
        if( k > n50_length) {
            n50 = kv_A(array, j);
            break;
        }
        j++;
    }
    mean = sum / count;
    median = kv_A(array, kv_size(array) / 2 );
    min = kv_A(array, 0);
    max = kv_A(array, kv_size(array) - 1);

    if(printFile)
    {
        printf("%s\t%d\t%lu\t%d\t%d\t%d\t%d\t%d\n", file, count, sum, min, max, mean, median, n50);
    } else {
        printf("%d\t%lu\t%d\t%d\t%d\t%d\t%d\n", count, sum, min, max, mean, median, n50);
    }
    gzclose(fp);
    kseq_destroy(seq);
    kv_destroy(array);
}

int main(int argc, char * argv[])
{
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

	/*if (optind > argc) {
        help();
		return 1;
	}*/
    if(argc - optind > 1)
    {
        // more than one input file, print the file name
        print_file = 1;
    }
    if( print_header) {
        if(print_file) {
            printf("file\t");
        }
        printf("count\tsum\tmin\tmax\tmean\tmedian\tn50\n");
    }
    int i;
    if(optind >= argc) {
        process("-", print_file);
    }
    else
    {
        for(i = optind; i < argc; ++i)
        {
            process(argv[i], print_file);
        }
    }
	return 0;
}
