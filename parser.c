#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "parser.h"
#include "printer.h"
#include "errors.h"
#include "sntchk.h"

#define COLOR_RED     "\x1b[1;31m"
#define COLOR_YELLOW  "\x1b[33m" // На будущее.
#define COLOR_RESET   "\x1b[0m"

static void print_error(const char *msg) {
    fprintf(stderr, COLOR_RED "[rclf] %s" COLOR_RESET "\n", msg);
}

static char* trim_quotes(char *str) {
    size_t len = strlen(str);
    if (len >= 2 && str[0] == '"' && str[len - 1] == '"') {
        str[len - 1] = '\0';
        memmove(str, str + 1, len - 1);
        return str;
    }
    return str;
}

static RclfValue parse_value(const char *text) {
    RclfValue val;
    val.is_array = false;
    val.value_count = 0;
    val.values = NULL;

    if (!text) return val;

    if (text[0] == '(' && text[strlen(text)-1] == ')') {
        val.is_array = true;

        char *copy = strdup(text + 1);
        if (!copy) return val;
        copy[strlen(copy) - 1] = '\0';

        char *token = strtok(copy, ",");
        while (token) {
            while (isspace(*token)) token++;
            char *trimmed = strdup(token);
            if (!trimmed) {
                free(copy);
                return val;
            }

            trim_quotes(trimmed);

            char **tmp = realloc(val.values, sizeof(char*) * (val.value_count + 1));
            if (!tmp) {
                free(trimmed);
                free(copy);
                return val;
            }

            val.values = tmp;
            val.values[val.value_count++] = trimmed;
            token = strtok(NULL, ",");
        }
        free(copy);
    } else {
        char *trimmed = strdup(text);
        if (!trimmed) return val;
        while (isspace(*trimmed)) trimmed++;
        trim_quotes(trimmed);

        val.values = malloc(sizeof(char*));
        if (!val.values) {
            free(trimmed);
            return val;
        }
        val.values[0] = trimmed;
        val.value_count = 1;
    }

    return val;
}

RclfDocument* rclf_parse(const char *filepath, bool check_syntax) {
    if (check_syntax) {
        int result = rclf_check_syntax(filepath);
        if (result != 0) return NULL;
    }

    FILE *fp = fopen(filepath, "r");
    if (!fp) return NULL;

    RclfDocument *doc = malloc(sizeof(RclfDocument));
    if (!doc) {
        fclose(fp);
        return NULL;
    }
    doc->column_count = 0;
    doc->columns = NULL;

    char line[2048];
    RclfColumn current = {0};
    bool in_column = false;
    bool used_indices[1024] = {0};

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        char *comment = strstr(line, "//");
        if (comment) *comment = '\0';
        if (strlen(line) < 2) continue;

        if (strncmp(line, "&c[", 3) == 0) {
            char *end = strchr(line + 3, ']');
            if (!end) {
                print_error("return NULL: wrong &c[n] section header"); // wrong &c usage
                fclose(fp);
                rclf_free(doc);
                return NULL;
            }
            *end = 0;
            int index = atoi(line + 3);
            if (used_indices[index]) {
                print_error("return NULL: duplicate column index detected"); // when there is two or more columns with same index
                fclose(fp);
                rclf_free(doc);
                return NULL;
            }
            used_indices[index] = true;
            in_column = true;
            current.index = index;
            current.key_count = 0;
            current.keys = NULL;
        } else if (strncmp(line, "&e", 2) == 0) {
            doc->columns = realloc(doc->columns, sizeof(RclfColumn) * (doc->column_count + 1));
            if (!doc->columns) {
                fclose(fp);
                rclf_free(doc);
                return NULL;
            }
            doc->columns[doc->column_count++] = current;
            in_column = false;
        } else if (in_column) {
            if (strchr(line, ';')) {
                char *token = strtok(line, ";");
                while (token) {
                    while (isspace(*token)) token++;
                    char *trimmed = strdup(token);
                    if (!trimmed) {
                        fclose(fp);
                        rclf_free(doc);
                        return NULL;
                    }
                    trim_quotes(trimmed);
                    current.keys = realloc(current.keys, sizeof(RclfKey) * (current.key_count + 1));
                    if (!current.keys) {
                        free(trimmed);
                        fclose(fp);
                        rclf_free(doc);
                        return NULL;
                    }
                    current.keys[current.key_count].key = trimmed;
                    current.keys[current.key_count].value_count = 0;
                    current.keys[current.key_count].values = NULL;
                    current.key_count++;
                    token = strtok(NULL, ";");
                }
            } else if (strchr(line, ':')) {
                char *token = strtok(line, ":");
                for (int i = 0; i < current.key_count && token; i++) {
                    while (isspace(*token)) token++;
                    RclfKey *key = &current.keys[i];
                    RclfValue val = parse_value(token);

                    RclfValue *tmp = realloc(key->values, sizeof(RclfValue) * (key->value_count + 1));
                    if (!tmp) {
                        fclose(fp);
                        rclf_free(doc);
                        return NULL;
                    }
                    key->values = tmp;
                    key->values[key->value_count++] = val;
                    token = strtok(NULL, ":");
                }
            }
        }
    }

    fclose(fp);
    return doc;
}
void rclf_free(RclfDocument *doc) {
    if (!doc) return;

    for (int c = 0; c < doc->column_count; c++) {
        RclfColumn col = doc->columns[c];
        for (int k = 0; k < col.key_count; k++) {
            free(col.keys[k].key);
            for (int v = 0; v < col.keys[k].value_count; v++) {
                for (int i = 0; i < col.keys[k].values[v].value_count; i++) {
                    free(col.keys[k].values[v].values[i]);
                }
                free(col.keys[k].values[v].values);
            }
            free(col.keys[k].values);
        }
        free(col.keys);
    }
    free(doc->columns);
    free(doc);
}
