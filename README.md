# ncount
![build status](https://github.com/mgualdron/ncount/actions/workflows/c-cpp.yml/badge.svg)

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
  -N, --csv-nl-count     output CSV records with embedded newlines
  -h, --help             This help
```

## Building ncount

The following projects have made `ncount` possible:

- [libcsv](https://github.com/rgamble/libcsv) - Version 3.0.3 of `libcsv` is included with `ncount`.
- [gnulib](https://www.gnu.org/software/gnulib/) - The `getline` module is included with `ncount` for portability.

Please consider contributing to those projects if you find `ncount` useful.

Note that this git repository does not include a `configure` script like a 
distribution tarball normally does.  If you don't want to bother with 
installing `autoconf` and `automake`, then download a distribution
[package](https://github.com/mgualdron/ncount/releases/download/v0.0.1/ncount-0.0.1.tar.gz)
and run `configure`:

```
./configure
```

If you want the `ncount` binary installed in your `$HOME/bin`, you should 
run something like:

```
./configure --prefix=$HOME
```

If you're building from a copy of this git repository, you'll need to have 
`autoconf` and `automake` installed on your system, and run the following 
command to generate a `configure` script:

```
autoreconf -i
```

...and subsequently run `configure` as mentioned before.

After `configure` completes successfully, you can do the usual:

```
make
make check
make install
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

## Author

Miguel Gualdron (dev at gualdron.com).
