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
    fprintf(stderr, "Usage:   fxsummary <in.fx>...\n\n");
}

int compare (const void * a, const void * b)
{
    return ( *(int*)a - *(int*)b );
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
        printf("count\tsum\tmn\tmax\tmean\tmedian\tn50\n");
    }
    int i;
    for(i = optind; i < argc; ++i)
    {
        fp = strcmp(argv[i], "-")? gzopen(argv[i], "r") : gzdopen(fileno(stdin), "r");
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

        if(print_file)
        {
            printf("%s\t%d\t%lu\t%d\t%d\t%d\t%d\t%d\n", argv[i], count, sum, min, max, mean, median, n50);
        } else {
            printf("%d\t%lu\t%d\t%d\t%d\t%d\t%d\n", count, sum, min, max, mean, median, n50);
        }
        gzclose(fp);
        kseq_destroy(seq);
        kv_destroy(array);
    }
	return 0;
}
