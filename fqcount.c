#include <zlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define BUFFER_LENGTH (1024*16)

void help()
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage:   fqcount <in.fq>...\n\n");
}

int main(int argc, char * argv[])
{
	gzFile fp;
    int print_file = 0;
    int c;
    while ((c = getopt (argc, argv, "hH")) != -1)
    {
        switch(c)
        {
            case 'H': print_file = 1; break;
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
    int i;
    char buffer[BUFFER_LENGTH]; // 16kb buffer
    for(i = optind; i < argc; ++i)
    {
        fp = strcmp(argv[i], "-")? gzopen(argv[i], "r") : gzdopen(fileno(stdin), "r");
        int count, l, c;
        count = 0;
        while ((l = gzread(fp, buffer, BUFFER_LENGTH)) > 0) {
            c = 0;
            while(c < l) {
                if(buffer[c] == '\n') {
                    count += 1;   
                }
                c++;
            }
        }
        if(print_file)
        {
            printf("%s\t%d\n", argv[i], count/4);
        } else {
            printf("%d\n", count/4);
        }
        gzclose(fp);
    }
	return 0;
}
