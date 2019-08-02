// -------------------------------------------------------------------------
// Program Name:    ncount.c
//
// Purpose:         To output records NOT having a given field count from
//                  the command-line.
//
// -------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "util/dbg.h"

static const char *program_name = "ncount";
static unsigned int fieldcount = 0;
static char *fieldcount_arg = 0;
static char *delim_arg = "\t";
static char *delim = "\t";

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
  -c, --field-count=FC   the field count to use while processing\n\
  -h, --help             This help\n\
");
    }

    exit (status);
}


static struct option long_options[] = {
    {"delimiter",   required_argument, 0, 'd'},
    {"field-count", required_argument, 0, 'c'},
    {"help",        no_argument,       0, 'h'},
    {0, 0, 0, 0}
};


/* Remove trailing CR/LF */
void chomp(char *s) {
    while(*s && *s != '\n' && *s != '\r') s++;
 
    *s = 0;
}


/*
   Process a regular delimited file.
   Output records not matching fieldcount.
*/
int ncount(char *filename)
{
    char *line = NULL;
    char *copy = NULL;
    char *token = NULL;
    FILE *fp = NULL;
    size_t len = 0;         // allocated size for line
    ssize_t bytes_read = 0; // num of chars read
    unsigned int fc = 0;

    if (filename[0] == '-') {
        fp = stdin;
    }
    else {
        fp = fopen(filename, "rb");
    }

    check(fp != NULL, "Error opening file: %s.", filename);

    while ((bytes_read = getline(&line, &len, fp)) != -1) {

        // strsep() will modify it's first argument, so we make a copy:
        copy = strdup(line);

        fc = 0;
        while ((token = strsep(&line, delim))) {
            fc++;
        }

        if (fc != fieldcount) {
            printf("%s", copy);
        }
        free(copy);
    }

    free(line);
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

    while (1) {

        // getopt_long stores the option index here.
        int option_index = 0;

        c = getopt_long (argc, argv, "hd:c:", long_options, &option_index);

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
        check(ncount(filename) == 0, "Error processing file: %s", filename);

        j++;

    } while (j < argc);

    return 0;

error:
    return -1;
}
