#ifndef RCLF_PARSER_H
#define RCLF_PARSER_H
#include <stdbool.h>
typedef struct {
    char **values;
    bool is_array;
    int value_count;
} RclfValue;
typedef struct {
    char *key;
    RclfValue *values;
    int value_count;
} RclfKey;
typedef struct {
    int index;
    RclfKey *keys;
    int key_count;
} RclfColumn;
typedef struct {
    RclfColumn *columns;
    int column_count;
} RclfDocument;
RclfDocument* rclf_parse(const char *filepath, bool check_syntax);
void rclf_free(RclfDocument *doc);
#endif
