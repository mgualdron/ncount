// -------------------------------------------------------------------------
// Program Name:    ncount.c
//
// Purpose:         To output records NOT having a given field count from
//                  the command-line.
//
// -------------------------------------------------------------------------
#define _GNU_SOURCE //cause stdio.h to include vasprintf
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "util/dbg.h"
#include <csv.h>

#define Sasprintf(write_to, ...) {           \
    char *tmp_string_to_extend = (write_to); \
    asprintf(&(write_to), __VA_ARGS__);      \
    free(tmp_string_to_extend);              \
}

static const char *program_name = "ncount";
static unsigned int fieldcount = 0;
static char *fieldcount_arg = 0;
static char *delim_arg = "\t";
static char *delim = "\t";
static char delim_csv = CSV_COMMA;
static char *quote_arg = NULL;
static char quote = CSV_QUOTE;
static int ignore_this = 0;

typedef struct { unsigned int rcount; unsigned int fcount; char *record; } CSV_status;

static void try_help (int status) {
    printf("Try '%s --help' for more information.\n", program_name);
    exit(status);
}

static void usage (int status) {
    if (status != 0) {
        try_help(status);
    }
    else {
      printf ("\
Usage: %s [OPTION]... [FILE]...\n\
", program_name);

      printf ("\
Output records from a delimited file NOT matching the given field count.\n\
More than one FILE can be specified.\n\
");

      printf ("\
\n\
  -d, --delimiter=DELIM  the delimiting character for the input FILE(s)\n\
  -n, --field-count=FC   the field count to use while processing (required)\n\
  -l, --add-line         include the line number in the output\n\
  -c, --add-count        include the field count in the output\n\
  -C  --csv              parse CSV files\n\
  -Q, --csv-quote        CSV quoting character (ignored unless --csv)\n\
  -h, --help             This help\n\
");
    }

    exit (status);
}


static struct option long_options[] = {
    {"delimiter",   required_argument, 0, 'd'},
    {"field-count", required_argument, 0, 'n'},
    {"add-line",    no_argument      , 0, 'l'},
    {"add-count",   no_argument      , 0, 'c'},
    {"csv",         no_argument      , 0, 'C'},
    {"csv-quote",   required_argument, 0, 'Q'},
    {"help",        no_argument      , 0, 'h'},
    {0, 0, 0, 0}
};


/* Return the number of delimiters in a string */
static unsigned int dcount(char *line, char *delim, const int dlen)
{
    int dc = 0;  // The delimiter count

    char *p = line;

    while((p = strstr(p, delim)))
    {
       dc++;
       p += dlen;
    }

    return dc;
}


/*
   Process a regular delimited file.
   Output records NOT matching fieldcount.
*/
static int ncount(char *filename)
{
    char *line = NULL;
    FILE *fp = NULL;
    size_t len = 0;         // allocated size for line
    ssize_t bytes_read = 0; // num of chars read
    const unsigned int dlen = strlen(delim);

    if (filename[0] == '-') {
        fp = stdin;
    }
    else {
        fp = fopen(filename, "rb");
    }

    check(fp != NULL, "Error opening file: %s.", filename);

    while ((bytes_read = getline(&line, &len, fp)) != -1) {

        if ( fieldcount != (dcount(line, delim, dlen) + 1) ) {
            printf("%s", line);
        }
    }

    free(line);
    fclose(fp);

    return 0;

error:
    return -1;
}

/* Version with line number added */
static int ncount_line(char *filename)
{
    char *line = NULL;
    FILE *fp = NULL;
    size_t len = 0;         // allocated size for line
    ssize_t bytes_read = 0; // num of chars read
    const unsigned int dlen = strlen(delim);
    unsigned int lnum = 0;

    if (filename[0] == '-') {
        fp = stdin;
    }
    else {
        fp = fopen(filename, "rb");
    }

    check(fp != NULL, "Error opening file: %s.", filename);

    while ((bytes_read = getline(&line, &len, fp)) != -1) {

        lnum++;
        if ( fieldcount != (dcount(line, delim, dlen) + 1) ) {
            printf("[rec:%d]%s%s", lnum, delim, line);
        }
    }

    free(line);
    fclose(fp);

    return 0;

error:
    return -1;
}

/* Version with field count added */
static int ncount_field(char *filename)
{
    char *line = NULL;
    FILE *fp = NULL;
    size_t len = 0;         // allocated size for line
    ssize_t bytes_read = 0; // num of chars read
    const unsigned int dlen = strlen(delim);
    unsigned int fc = 0;

    if (filename[0] == '-') {
        fp = stdin;
    }
    else {
        fp = fopen(filename, "rb");
    }

    check(fp != NULL, "Error opening file: %s.", filename);

    while ((bytes_read = getline(&line, &len, fp)) != -1) {

        fc = dcount(line, delim, dlen) + 1;
        if ( fieldcount != fc ) {
            printf("[fields:%d]%s%s", fc, delim, line);
        }
    }

    free(line);
    fclose(fp);

    return 0;

error:
    return -1;
}

/* Version with line num and field count added */
static int ncount_line_field(char *filename)
{
    char *line = NULL;
    FILE *fp = NULL;
    size_t len = 0;         // allocated size for line
    ssize_t bytes_read = 0; // num of chars read
    const unsigned int dlen = strlen(delim);
    unsigned int lnum = 0;
    unsigned int fc = 0;

    if (filename[0] == '-') {
        fp = stdin;
    }
    else {
        fp = fopen(filename, "rb");
    }

    check(fp != NULL, "Error opening file: %s.", filename);

    while ((bytes_read = getline(&line, &len, fp)) != -1) {

        lnum++;
        fc = dcount(line, delim, dlen) + 1;
        if ( fieldcount != fc ) {
            printf("[rec:%d]%s[fields:%d]%s%s", lnum, delim, fc, delim, line);
        }
    }

    free(line);
    fclose(fp);

    return 0;

error:
    return -1;
}

// Callback 1 for CSV support, called whenever a field is processed:
void cb1 (void *s, size_t len, void *data)
{
    size_t fld_size;
    char *fld = (char *)s;
    CSV_status *csv_track = (CSV_status *)data;

    csv_track->fcount++;
    fld_size = csv_write2(NULL, 0, len ? fld : "", len, quote);
    char *out_temp = (char *)malloc((fld_size + 1) * sizeof(char));
    csv_write2(out_temp, fld_size, len ? fld : "", len, quote);
    out_temp[fld_size] = '\0';  // NUL-terminate the written field
    if ( csv_track->record == NULL ) {
        //csv_track->record = strdup("");
        Sasprintf(csv_track->record, "%s", out_temp);
    }
    else {
        Sasprintf(csv_track->record, "%s%c%s", csv_track->record, delim_csv, out_temp);
    }
    free(out_temp);
}

// A function pointer to one of the cb2 functions below:
void (*cb2) (int, void *);

// Callback 2 for CSV support, called whenever a record is processed:
void cb2_none (int c, void *data)
{
    CSV_status *csv_track = (CSV_status *)data;

    csv_track->rcount++;
    if ( fieldcount != csv_track->fcount ) {
        printf("%s\n", csv_track->record);
    }

    csv_track->fcount = 0;
    free(csv_track->record);
    csv_track->record = NULL;
    ignore_this = c;
}

// Callback 2 for CSV support, called whenever a record is processed:
void cb2_line (int c, void *data)
{
    CSV_status *csv_track = (CSV_status *)data;

    csv_track->rcount++;
    if ( fieldcount != csv_track->fcount ) {
        printf("[rec:%d]%c%s\n", csv_track->rcount, delim_csv, csv_track->record);
    }

    csv_track->fcount = 0;
    free(csv_track->record);
    csv_track->record = NULL;
    ignore_this = c;
}

// Callback 2 for CSV support, called whenever a record is processed:
void cb2_field (int c, void *data)
{
    CSV_status *csv_track = (CSV_status *)data;

    csv_track->rcount++;
    if ( fieldcount != csv_track->fcount ) {
        printf("[fields:%d]%c%s\n", csv_track->fcount, delim_csv, csv_track->record);
    }

    csv_track->fcount = 0;
    free(csv_track->record);
    csv_track->record = NULL;
    ignore_this = c;
}

// Callback 2 for CSV support, called whenever a record is processed:
void cb2_line_field (int c, void *data)
{
    CSV_status *csv_track = (CSV_status *)data;

    csv_track->rcount++;
    if ( fieldcount != csv_track->fcount ) {
        printf("[rec:%d]%c[fields:%d]%c%s\n", csv_track->rcount, delim_csv, csv_track->fcount, delim_csv, csv_track->record);
    }

    csv_track->fcount = 0;
    free(csv_track->record);
    csv_track->record = NULL;
    ignore_this = c;
}

int ncount_csv(char *filename)
{
    struct csv_parser p;
    char buf[1024];
    FILE *fp = NULL;
    size_t bytes_read = 0; // num of chars read
    CSV_status *csv_track = (CSV_status *)malloc(sizeof(CSV_status));

    csv_track->rcount = 0;
    csv_track->fcount = 0;
    csv_track->record = NULL;

    if (filename[0] == '-') {
        fp = stdin;
    }
    else {
        fp = fopen(filename, "rb");
    }

    check(fp != NULL, "Error opening file: %s.", filename);

    check(csv_init(&p, CSV_APPEND_NULL) == 0, "Error initializing CSV parser.");

    // Set some parsing params:
    csv_set_delim(&p, delim_csv);
    csv_set_quote(&p, quote);

    while ((bytes_read=fread(buf, 1, 1024, fp)) > 0) {
        check(csv_parse(&p, buf, bytes_read, cb1, cb2, csv_track) == bytes_read, "Error while parsing file: %s", csv_strerror(csv_error(&p)));
    }

    check(csv_fini(&p, cb1, cb2, csv_track) == 0, "Error finishing CSV processing.");

    csv_free(&p);
    free(csv_track);

    fclose(fp);

    return 0;

error:
    return -1;
}


/* The main function */
int main (int argc, char *argv[])
{
    int c;
    int delim_arg_flag = 0;
    int fieldcount_arg_flag = 0;
    int add_lnum_arg_flag = 0;
    int add_fc_arg_flag = 0;
    int csv_mode = 0;

    while (1) {

        // getopt_long stores the option index here.
        int option_index = 0;

        c = getopt_long (argc, argv, "hlCcd:n:", long_options, &option_index);

        // Detect the end of the options.
        if (c == -1) break;

        switch (c) {
            case 0:
                // If this option set a flag, do nothing else now.
                break;

            case 'n':
                debug("option -n with value `%s'", optarg);
                fieldcount_arg      = optarg;
                fieldcount_arg_flag = 1;
                break;

            case 'd':
                debug("option -d with value `%s'", optarg);
                delim_arg = optarg;
                delim_arg_flag = 1;
                break;

            case 'l':
                debug("option -l");
                add_lnum_arg_flag = 1;
                break;

            case 'c':
                debug("option -c");
                add_fc_arg_flag = 1;
                break;

            case 'C':
                debug("option -C");
                csv_mode = 1;
                break;

            case 'Q':
                debug("option -Q");
                quote_arg = optarg;
                check(strlen(quote_arg) == 1, "ERROR: CSV quoting character must be exactly one byte long");
                quote = quote_arg[0];
                break;

            case 'h':
                debug("option -h");
                usage(0);
                break;

            case ':':   /* missing option argument */
                fprintf(stderr, "%s: option '-%c' requires an argument\n",
                        argv[0], optopt);
                break;

            // getopt_long already printed an error message.
            case '?':
                usage(1);
                break;

            default:
                usage(1);
        }
    }

    if (csv_mode && delim_arg_flag) {
        check(strlen(delim_arg) == 1, "ERROR: CSV delimiter must be exactly one byte long");
        delim_csv = delim_arg[0];
    }
    else if (!csv_mode && delim_arg_flag) {
        delim = delim_arg;
    }

    if (fieldcount_arg_flag) {
        fieldcount = (unsigned int) strtol(fieldcount_arg, (char **)NULL, 10);
    }

    check(fieldcount > 0, "ERROR: Please specify a valid field count with -n");

    int j = optind;  // A copy of optind (the number of options at the command-line),
                     // which is not the same as argc, as that counts ALL
                     // arguments.  (optind <= argc).

    // Process any remaining command line arguments (input files).
    do {

        char *filename = NULL;

        // Assume STDIN if no additional arguments, else loop through them:
        if (optind == argc) {
            filename = "-";
        }
        else if (optind < argc) {
            filename = argv[j];
        }
        else if (optind > argc) {
            break;
        }

        debug("The filename is %s", filename);

        // Process the file:
        if (csv_mode) {
            if (add_lnum_arg_flag && add_fc_arg_flag) {
                cb2 = cb2_line_field;
            }
            else if (add_fc_arg_flag) {
                cb2 = cb2_field;
            }
            else if (add_lnum_arg_flag) {
                cb2 = cb2_line;
            }
            else {
                cb2 = cb2_none;
            }
            check(ncount_csv(filename) == 0, "Error in CSV-mode processing of file: %s", filename);
        }
        else {
            if (add_lnum_arg_flag && add_fc_arg_flag) {
                check(ncount_line_field(filename) == 0, "Error processing file: %s", filename);
            }
            else if (add_fc_arg_flag) {
                check(ncount_field(filename) == 0, "Error processing file: %s", filename);
            }
            else if (add_lnum_arg_flag) {
                check(ncount_line(filename) == 0, "Error processing file: %s", filename);
            }
            else {
                check(ncount(filename) == 0, "Error processing file: %s", filename);
            }
        }

        j++;

    } while (j < argc);

    return 0;

error:
    return -1;
}
