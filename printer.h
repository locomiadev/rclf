#ifndef RCLF_PRINTER_H
#define RCLF_PRINTER_H
#include "parser.h"
void rclf_print_all(RclfDocument *doc);
void rclf_print_column(RclfDocument *doc, int col_index);
void rclf_print_key(RclfKey key, int col_index, int key_index, int quote_pos);
void rclf_print_value(RclfValue val, int col_index, int key_index, int value_index, int quote_pos);
void print_bool(RclfValue *val, int col, int key, int val_dx, int indent);
#endif
