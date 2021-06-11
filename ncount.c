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
static unsigned int running_fieldcount = 0;
static unsigned int linecount = 0;
static char *fieldcount_arg = 0;
static char *delim_arg = "\t";
static char *delim = "\t";
static char delim_csv = CSV_COMMA;
static char *quote_arg = NULL;
static char quote = CSV_QUOTE;
static char *output_rec = NULL;

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
  -c, --field-count=FC   the field count to use while processing (required)\n\
  -l, --add-line         include the line number in the output\n\
  -C, --add-count        include the field count in the output\n\
      --csv              parse CSV files\n\
  -Q, --csv-quote        CSV quoting character (ignored unless --csv)\n\
  -h, --help             This help\n\
");
    }

    exit (status);
}


static struct option long_options[] = {
    {"delimiter",   required_argument, 0, 'd'},
    {"field-count", required_argument, 0, 'c'},
    {"add-line",    no_argument      , 0, 'l'},
    {"add-count",   no_argument      , 0, 'C'},
    {"csv",       no_argument,       0, 'S'},
    {"csv-quote", required_argument, 0, 'Q'},
    {"help",        no_argument,       0, 'h'},
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

    running_fieldcount++;
    if ( output_rec == NULL ) { output_rec = strdup(""); }
    fld_size = csv_write2(NULL, 0, len ? s : "", len, quote) + 1;
    char *out_temp = (char *)malloc(fld_size * sizeof(char));
    csv_write2(out_temp, fld_size, len ? s : "", len, quote);
    Sasprintf(output_rec, "%s%c%s", output_rec, delim_csv, out_temp);
    free(out_temp);
}

// Callback 2 for CSV support, called whenever a record is processed:
void cb2 (int c, void *data)
{
    linecount++;
    if ( fieldcount != running_fieldcount ) {
        printf("[rec:%d]%c[fields:%d]%s\n", linecount, delim_csv, running_fieldcount, output_rec);
    }
    running_fieldcount = 0;
    free(output_rec);
    output_rec = NULL;
}

int ncount_csv(char *filename)
{
    struct csv_parser p;
    char buf[1024];
    FILE *fp = NULL;
    size_t bytes_read = 0; // num of chars read

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
        check(csv_parse(&p, buf, bytes_read, cb1, cb2, NULL) == bytes_read, "Error while parsing file: %s", csv_strerror(csv_error(&p)));
    }

    check(csv_fini(&p, cb1, cb2, NULL) == 0, "Error finishing CSV processing.");

    csv_free(&p);

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

        c = getopt_long (argc, argv, "hlCd:c:", long_options, &option_index);

        // Detect the end of the options.
        if (c == -1) break;

        switch (c) {
            case 0:
                // If this option set a flag, do nothing else now.
                break;

            case 'c':
                debug("option -c with value `%s'", optarg);
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

            case 'C':
                debug("option -C");
                add_fc_arg_flag = 1;
                break;

            case 'S':
                debug("option -S");
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

    if (delim_arg_flag) {
        delim = delim_arg;
    }

    if (fieldcount_arg_flag) {
        fieldcount = (unsigned int) strtol(fieldcount_arg, (char **)NULL, 10);
    }

    check(fieldcount > 0, "ERROR: Please specify a valid field count with -c");

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
