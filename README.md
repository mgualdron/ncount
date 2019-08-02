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
  -c, --field-count=FC   the field count to use while processing
  -h, --help             This help
```

## BUILD

Type the following within the source directory:

```
make ncount
```

Copy the binary somewhere in your path, like:

```
cp ncount ~/bin
```

## Why?

Speed.

```
time awk -F$'\t' '{if ( NF != 19 ) print $0}' 50_million_row_file.txt

real    1m59.619s
user    1m58.566s
sys     0m1.052s

time ncount -c 19 50_million_row_file.tct

real    0m17.425s
user    0m12.554s
sys     0m4.871s
```


## TODO

- Optionally add the line number to each record in the output.
- Support for CSV files.