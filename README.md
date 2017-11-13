# fx_tools
Very small utilities for interrogating fasta and fastq files.

## facount
count the records in a fasta file. Equivelent to `grep -c '^>' file.fa`

## fqcount
count the records in a fastq file.

## fxfilter
only keep reads if they meet certain length of GC requirements

## fxsummary
Print tabular summary statistics on a whole file. The default columns
are count, sum, min, max, mean, median, n50

## fxuniq
remove records that have identical header names
