#ifndef RCLF_PRINTER_H
#define RCLF_PRINTER_H

#include "parser.h"
#include <stdbool.h>

typedef struct {
    bool use_colors;
    bool brief_output;
} PrintOptions;

void rclf_print_all(RclfDocument *doc, PrintOptions opts);
void rclf_print_column(RclfDocument *doc, int col_index, PrintOptions opts);
void rclf_print_key(RclfKey key, int col_index, int key_index, int quote_pos, PrintOptions opts);
void rclf_print_value(RclfValue val, int col_index, int key_index, int value_index, int quote_pos, PrintOptions opts);

#endif