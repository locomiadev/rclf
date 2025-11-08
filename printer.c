#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include "printer.h"
#include "parser.h"

static bool is_boolean(const char *val) {
    if (!val) return false;
    return (strcasecmp(val, "true") == 0 || 
            strcasecmp(val, "false") == 0 ||
            strcasecmp(val, "yes") == 0 || 
            strcasecmp(val, "no") == 0);
}

void rclf_print_value(RclfValue val, int col_index, int key_index, int value_index, int quote_pos, PrintOptions opts) {
    if (opts.brief_output) {
        if (val.is_array) {
            for (int j = 0; j < val.value_count; j++) {
                printf("%s\n", val.values[j]);
            }
        } else {
            printf("%s\n", val.values[0]);
        }
        return;
    }

    char prefix[32];
    snprintf(prefix, sizeof(prefix), "Col%d %d %d~ ", col_index, key_index, value_index);
    int prefix_len = strlen(prefix);

    if (val.is_array) {
        for (int j = 0; j < val.value_count; j++) {
            if (is_boolean(val.values[j])) {
                printf("%s%*s%s%s%s\n", prefix, quote_pos - prefix_len, "",
                       opts.use_colors ? "\x1b[1m" : "", val.values[j],
                       opts.use_colors ? "\x1b[0m" : "");
            } else {
                printf("%s%*s%s\"%s\"%s\n", prefix, quote_pos - prefix_len, "",
                       opts.use_colors ? "\x1b[1m" : "", val.values[j],
                       opts.use_colors ? "\x1b[0m" : "");
            }
        }
    } else {
        if (is_boolean(val.values[0])) {
            printf("%s%*s%s%s%s\n", prefix, quote_pos - prefix_len, "",
                   opts.use_colors ? "\x1b[1m" : "", val.values[0],
                   opts.use_colors ? "\x1b[0m" : "");
        } else {
            printf("%s%*s%s\"%s\"%s\n", prefix, quote_pos - prefix_len, "",
                   opts.use_colors ? "\x1b[1m" : "", val.values[0],
                   opts.use_colors ? "\x1b[0m" : "");
        }
    }
}

void rclf_print_key(RclfKey key, int col_index, int key_index, int quote_pos, PrintOptions opts) {
    if (opts.brief_output) {
        printf("%s\n", key.key);
        return;
    }

    char prefix[32];
    snprintf(prefix, sizeof(prefix), "Col%d %d/ ", col_index, key_index);
    int prefix_len = strlen(prefix);
    printf("%s%*s%s\"%s\"%s\n", prefix, quote_pos - prefix_len, "",
           opts.use_colors ? "\x1b[1m" : "", key.key,
           opts.use_colors ? "\x1b[0m" : "");

    for (int i = 0; i < key.value_count; i++) {
        rclf_print_value(key.values[i], col_index, key_index, i, quote_pos, opts);
    }
}

void rclf_print_column(RclfDocument *doc, int col_index, PrintOptions opts) {
    if (!doc || col_index >= doc->column_count) return;
    RclfColumn col = doc->columns[col_index];

    int quote_pos = 12;

    for (int k = 0; k < col.key_count; k++) {
        rclf_print_key(col.keys[k], col.index, k, quote_pos, opts);
    }
}

void rclf_print_all(RclfDocument *doc, PrintOptions opts) {
    if (!doc) return;
    for (int i = 0; i < doc->column_count; i++) {
        rclf_print_column(doc, i, opts);
    }
}