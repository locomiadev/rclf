#include <stdio.h>
#include <string.h>
#include "parser.h"
void rclf_print_value(RclfValue val, int col_index, int key_index, int value_index, int quote_pos) {
    char prefix[32];
    snprintf(prefix, sizeof(prefix), "Col%d %d %d~ ", col_index, key_index, value_index);
    int prefix_len = strlen(prefix);

    if (val.is_array) {
        for (int j = 0; j < val.value_count; j++) {
            printf("%s%*s\"%s\"\n", prefix, quote_pos - prefix_len, "", val.values[j]);
        }
    } else {
        if (val.value_count > 0 && val.values[0])
            printf("%s%*s\"%s\"\n", prefix, quote_pos - prefix_len, "", val.values[0]);
    }
}
void rclf_print_key(RclfKey key, int col_index, int key_index, int quote_pos) {
    for (int i = 0; i < key.value_count; i++) {
        rclf_print_value(key.values[i], col_index, key_index, i, quote_pos);
    }
}
void rclf_print_column(RclfDocument *doc, int col_index) {
    if (!doc || col_index >= doc->column_count) return;
    RclfColumn col = doc->columns[col_index];

    char key_prefix[32];
    snprintf(key_prefix, sizeof(key_prefix), "Col%d %d/ ", col.index, col_index);
    int key_prefix_len = strlen(key_prefix);

    char value_prefix[32];
    snprintf(value_prefix, sizeof(value_prefix), "Col%d %d %d~ ", col.index, 0, 0);
    int value_prefix_len = strlen(value_prefix);

    int quote_pos = value_prefix_len;
    if (key_prefix_len > quote_pos) quote_pos = key_prefix_len;
    quote_pos += 2;

    for (int k = 0; k < col.key_count; k++) {
        char *key = col.keys[k].key;
        snprintf(key_prefix, sizeof(key_prefix), "Col%d %d/ ", col.index, k);
        key_prefix_len = strlen(key_prefix);
        printf("%s%*s\"%s\"\n", key_prefix, quote_pos - key_prefix_len, "", key);
        rclf_print_key(col.keys[k], col.index, k, quote_pos);
    }
}
void rclf_print_all(RclfDocument *doc) {
    if (!doc) return;
    for (int i = 0; i < doc->column_count; i++) {
        rclf_print_column(doc, i);
    }
}

