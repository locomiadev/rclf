#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"
#include "printer.h"
#include "errors.h"
#include "sntchk.h"

static void print_usage(void) {
    printf("usage:\n");
    printf("  rclf out [-n] [-C] [-b] -f <file name> [-c N] [-k N] [-v N]\n\n");
    printf("  rclf version\n\n");
    printf("options:\n");
    printf("  -f <file name>  Path to RCLF document\n");
    printf("  -n              Disable syntax checking\n");
    printf("  -C              Disable colors\n");
    printf("  -b              Nonbloated output\n");
    printf("  -c <N>          Column index (optional)\n");
    printf("  -k <N>          Key index (requires -c)\n");
    printf("  -v <N>          Value index (requires -c & -k)\n");
}

int main(int argc, char *argv[]) {
    const char *command = NULL;
    const char *filepath = NULL;
    int col_index = -1;
    int key_index = -1;
    int val_index = -1;
    bool check_syntax = true;
    
    PrintOptions print_opts = {
        .use_colors = true,
        .brief_output = false
    };

    if (argc > 1 && strcmp(argv[1], "version") == 0) {
        printf("  rclf: %s\033[1m\033[94mversion 0.3\033[0m\n",
               print_opts.use_colors ? "" : "");
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "out") == 0) {
            command = "out";
        } else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            filepath = argv[++i];
        } else if (strcmp(argv[i], "-n") == 0) {
            check_syntax = false;
        } else if (strcmp(argv[i], "-C") == 0) {
            print_opts.use_colors = false;
        } else if (strcmp(argv[i], "-b") == 0) {
            print_opts.brief_output = true;
        } else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            col_index = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-k") == 0 && i + 1 < argc) {
            key_index = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-v") == 0 && i + 1 < argc) {
            val_index = atoi(argv[++i]);
        } else {
            return error_invalid_args(argv[i]);
        }
    }

    if (!command || !filepath) {
        print_usage();
        return error_invalid_args("");
    }

    if (key_index != -1 && col_index == -1) {
        print_usage();
        return error_invalid_args("-k requires -c");
    }
    if (val_index != -1 && (col_index == -1 || key_index == -1)) {
        print_usage();
        return error_invalid_args("-v requires -c and -k");
    }

    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        return error_file_not_found(filepath);
    }
    fclose(fp);

    if (!print_opts.brief_output) {
        printf("[rclf] reading \"%s\"...\n", filepath);
    }
    
    RclfDocument *doc = rclf_parse(filepath, check_syntax);
    if (!doc) {
        return error_parsing_failed();
    }

    int actual_col_idx = -1;
    if (col_index != -1) {
        for (int i = 0; i < doc->column_count; i++) {
            if (doc->columns[i].index == col_index) {
                actual_col_idx = i;
                break;
            }
        }
        if (actual_col_idx == -1) {
            rclf_free(doc);
            return error_missing_column_index(col_index);
        }
    }

    if (actual_col_idx != -1 && key_index != -1) {
        if (key_index < 0 || key_index >= doc->columns[actual_col_idx].key_count) {
            rclf_free(doc);
            return error_invalid_args("-k");
        }
        if (val_index != -1) {
            if (val_index < 0 || val_index >= doc->columns[actual_col_idx].keys[key_index].value_count) {
                rclf_free(doc);
                return error_invalid_args("-v");
            }
        }
    }

    if (col_index == -1) {
        rclf_print_all(doc, print_opts);
    } else if (key_index == -1) {
        rclf_print_column(doc, actual_col_idx, print_opts);
    } else if (val_index == -1) {
        rclf_print_key(doc->columns[actual_col_idx].keys[key_index],
                      col_index, key_index, 10, print_opts);
    } else {
        rclf_print_value(doc->columns[actual_col_idx].keys[key_index].values[val_index],
                        col_index, key_index, val_index, 10, print_opts);
    }

    rclf_free(doc);
    return 0;
}
