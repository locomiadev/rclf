#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "parser.h"
#include "errors.h"
#include "sntchk.h"

static char *trim_ws(char *s) {
    if (!s) return s;
    char *start = s;
    while (*start && isspace((unsigned char)*start)) start++;
    if (*start == '\0') return start;
    char *end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
    return start;
}

static char *strip_quotes(char *s) {
    char *p = trim_ws(s);
    size_t len = strlen(p);
    if (len >= 2 && p[0] == '"' && p[len - 1] == '"') {
        p[len - 1] = '\0';
        return p + 1;
    }
    return p;
}

static char **split_fields(char *line, const char *delim, int *count) {
    char **res = NULL;
    int cap = 0;
    int n = 0;
    
    char *tmp = strdup(line);
    if (!tmp) return NULL;
    
    char *s = tmp;
    char *token;
    bool in_quotes = false;
    int start = 0;
    
    for (int i = 0; s[i]; i++) {
        if (s[i] == '"') in_quotes = !in_quotes;
        if (!in_quotes && strchr(delim, s[i])) {
            s[i] = '\0';
            token = trim_ws(s + start);
            if (*token) {
                if (n >= cap) {
                    int newcap = cap == 0 ? 4 : cap * 2;
                    char **tmp = realloc(res, newcap * sizeof(char*));
                    if (!tmp) goto err;
                    res = tmp;
                    cap = newcap;
                }
                res[n] = strdup(token);
                if (!res[n]) goto err;
                n++;
            }
            start = i + 1;
        }
    }
    
    token = trim_ws(s + start);
    if (*token) {
        if (n >= cap) {
            char **tmp = realloc(res, (cap + 1) * sizeof(char*));
            if (!tmp) goto err;
            res = tmp;
            cap++;
        }
        res[n] = strdup(token);
        if (!res[n]) goto err;
        n++;
    }
    
    free(tmp);
    *count = n;
    return res;

err:
    free(tmp);
    if (res) {
        for (int i = 0; i < n; i++) free(res[i]);
        free(res);
    }
    *count = 0;
    return NULL;
}

RclfDocument* rclf_parse(const char *filepath, bool check_syntax) {
    if (check_syntax && rclf_check_syntax(filepath) != 0) return NULL;

    FILE *fp = fopen(filepath, "r");
    if (!fp) return NULL;

    RclfDocument *doc = malloc(sizeof(RclfDocument));
    if (!doc) { fclose(fp); return NULL; }
    doc->columns = NULL;
    doc->column_count = 0;

    char line[2048];
    RclfColumn current = {0};
    bool in_column = false;

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        char *p = trim_ws(line);
        if (*p == '\0' || (p[0] == '/' && p[1] == '/')) continue;

        if (strncmp(p, "&c[", 3) == 0) {
            char *br = strchr(p, '[');
            char *br2 = strchr(p, ']');
            if (!br || !br2 || br2 <= br) continue;
            *br2 = '\0';
            int idx = atoi(br + 1);
            current.index = idx;
            current.keys = NULL;
            current.key_count = 0;
            in_column = true;
        }
        else if (strncmp(p, "&e", 2) == 0) {
            if (in_column) {
                RclfColumn *tmp = realloc(doc->columns, (doc->column_count + 1) * sizeof(RclfColumn));
                if (!tmp) goto err;
                doc->columns = tmp;
                doc->columns[doc->column_count++] = current;
            }
            in_column = false;
        }
        else if (in_column) {
            int count;
            char **fields;
            
            if (strchr(p, ';')) {
                fields = split_fields(p, ";", &count);
                if (!fields) goto err;
                
                RclfKey *tmp = realloc(current.keys, (current.key_count + count) * sizeof(RclfKey));
                if (!tmp) { 
                    for (int i = 0; i < count; i++) free(fields[i]);
                    free(fields);
                    goto err;
                }
                current.keys = tmp;
                
                for (int i = 0; i < count; i++) {
                    current.keys[current.key_count + i].key = strdup(strip_quotes(fields[i]));
                    current.keys[current.key_count + i].values = NULL;
                    current.keys[current.key_count + i].value_count = 0;
                }
                current.key_count += count;
                
                for (int i = 0; i < count; i++) free(fields[i]);
                free(fields);
            }
            else if (strchr(p, ':')) {
                fields = split_fields(p, ":", &count);
                if (!fields || count != current.key_count) {
                    if (fields) {
                        for (int i = 0; i < count; i++) free(fields[i]);
                        free(fields);
                    }
                    continue;
                }
                
                for (int i = 0; i < count; i++) {
                    char *val = strip_quotes(fields[i]);
                    RclfValue v = {0};
                    v.values = malloc(sizeof(char*));
                    if (!v.values) {
                        for (int j = 0; j < count; j++) free(fields[j]);
                        free(fields);
                        goto err;
                    }
                    v.values[0] = strdup(val);
                    v.value_count = 1;
                    
                    RclfValue *tmp = realloc(current.keys[i].values, 
                        (current.keys[i].value_count + 1) * sizeof(RclfValue));
                    if (!tmp) {
                        free(v.values[0]);
                        free(v.values);
                        for (int j = 0; j < count; j++) free(fields[j]);
                        free(fields);
                        goto err;
                    }
                    current.keys[i].values = tmp;
                    current.keys[i].values[current.keys[i].value_count++] = v;
                }
                
                for (int i = 0; i < count; i++) free(fields[i]);
                free(fields);
            }
        }
    }

    fclose(fp);
    return doc;

err:
    fclose(fp);
    rclf_free(doc);
    return NULL;
}

void rclf_free(RclfDocument *doc) {
    if (!doc) return;
    for (int c = 0; c < doc->column_count; c++) {
        for (int k = 0; k < doc->columns[c].key_count; k++) {
            free(doc->columns[c].keys[k].key);
            for (int v = 0; v < doc->columns[c].keys[k].value_count; v++) {
                for (int i = 0; i < doc->columns[c].keys[k].values[v].value_count; i++)
                    free(doc->columns[c].keys[k].values[v].values[i]);
                free(doc->columns[c].keys[k].values[v].values);
            }
            free(doc->columns[c].keys[k].values);
        }
        free(doc->columns[c].keys);
    }
    free(doc->columns);
    free(doc);
}
















// Здравствуйте, я великий уникальный, не такой как все Mr.Sugoma.
// Я поселился в данную программу потому-что в Говниково-1 отключили интернет.
// Пожалуйста, купите мне нутеллу.
// NOOOCOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOORD!
