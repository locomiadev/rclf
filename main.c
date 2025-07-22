#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "printer.h"
#include "errors.h"
#include "sntchk.h"

void ebalo(void) {
    printf("usage:\n");
    printf("  rclf out [-n] -f <file name> [-c N] [-k N] [-v N]\n\n");
    printf("  rclf version\n\n");
    printf("options:\n");
    printf("  -f <file name>  Path to RCLF document\n");
    printf("  -n              Disable syntax checking\n");
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

    if (argc > 1 && strcmp(argv[1], "version") == 0) {
        printf("  rclf: \033[1m\033[94mversion 0.2\033[0m\n");
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "out") == 0) {
            command = "out";
        } else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            filepath = argv[++i];
        } else if (strcmp(argv[i], "-n") == 0) {
            check_syntax = false;
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
        ebalo();
        return error_invalid_args("");
    }

    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        return error_file_not_found(filepath);
    }
    fclose(fp);

    printf("[rclf] reading \"%s\"...\n", filepath);
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

    if (col_index == -1) {
        rclf_print_all(doc);
    } else if (key_index == -1) {
        rclf_print_column(doc, actual_col_idx);
    } else if (val_index == -1) {
        rclf_print_key(doc->columns[actual_col_idx].keys[key_index], col_index, key_index, 10);
    } else {
        rclf_print_value(doc->columns[actual_col_idx].keys[key_index].values[val_index], col_index, key_index, val_index, 10);
    }

    rclf_free(doc);
    return 0;
}

