/*
 * Test codes for oracsv.
 * author: Daekyu Lee <dklee@yidigun.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "config.h"
#define VERSION "0.0.1"

char* MYNAME;
const char* ARGSPEC = "vhqD:";

static void print_error(const char*, const int);
static void print_version(void);
static void print_help(FILE*);

void main(int argc, char* argv[]) {

    int opt = '0', cfg_quiet = 0;
    char* cfg_filename = NULL;
    FILE* cfg_file = NULL;

    // program name
    MYNAME = argv[0];
    while ((opt = getopt(argc, argv, ARGSPEC)) != -1) {
        switch (opt) {
        case 'v':
            print_version();
            exit(0);
            break;
        case 'h':
            print_help(stderr);
            exit(0);
            break;
        case 'q':
            cfg_quiet = 1;
            break;
        case 'D':
            printf("-D %s\n", optarg);
            break;
        default:
            print_help(stderr);
            exit(1);
        }
    }

    if (optind < argc) {
        cfg_filename = argv[optind];
    }
    
    if (cfg_filename != NULL && strcmp(cfg_filename, "-") == 0) {
        cfg_file = stdin;
    }
    else if (cfg_filename != NULL) {
        cfg_file = fopen(cfg_filename, "r");
        if (cfg_file == NULL) {
            print_error("can't open config file", 1);
            exit(1);
        }
    }

    cfg_config_t* config = cfg_create(CFG_SRC_TYPE_FILE, cfg_file);
    if (config == NULL) {
        print_error("can't create config", 0);
        exit(1);
    }

    // TODO: -D param apply

    // TODO: validate config

    // TODO: dump config for debug
    printf("quiet mode: %d\n", cfg_quiet);
    printf("config file: %s\n", cfg_filename);

    // TODO: export

    exit(0);
}

static void print_error(const char* message, const int use_perror) {

    if (use_perror) {
        char buf[1024];
        sprintf(buf, "%s: %s", MYNAME, message);
        perror(buf);
    }
    else {
        fprintf(stderr, "%s: %s", MYNAME, message);
    }
}

static void print_version(void) {
    printf("%s v%s\n", MYNAME, VERSION);
}

static void print_help(FILE* stream) {
    FILE* out = (stream == NULL)? stdout: stream;

    fprintf(out, "Usage: %s [ -vhq ] [ -D name=value ... ] [ task-config-file ]\n", MYNAME);
    fprintf(out, "  -v : print version and exit.\n");
    fprintf(out, "  -h : print help text and exit.\n");
    fprintf(out, "  -q : suppress log output.\n");
    fprintf(out, "  -D : specify task configuration value.\n");
    fprintf(out, "  task-config-file : specify task configuration filename.\n");
    fprintf(out, "\n");
}
