# ncount

## Synopsis

A trivial command-line program to print lines from a delimited file that 
don't match a given field count.

```
ncount -h

Usage: ncount [OPTION]... [FILE]...
Output records from a delimited file NOT matching the given field count.
More than one FILE can be specified.

  -d, --delimiter=DELIM  the delimiting character for the input FILE(s)
  -n, --field-count=FC   the field count to use while processing (required)
  -l, --add-line         include the line number in the output
  -c, --add-count        include the field count in the output
  -C  --csv              parse CSV files
  -Q, --csv-quote        CSV quoting character (ignored unless --csv)
  -h, --help             This help
```

## Build

Type the following within the source directory:

```
make ncount
```

Move the binary somewhere in your path, like:

```
mv ncount ~/bin
```

## Why?

Speed.

```
time awk -F$'\t' '{if ( NF != 19 ) print $0}' 50_million_row_file.txt

real    1m59.619s
user    1m58.566s
sys     0m1.052s

time ncount -n 19 50_million_row_file.txt

real    0m17.425s
user    0m12.554s
sys     0m4.871s
```
