#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define STB_C_LEXER_IMPLEMENTATION
#include "stb_c_lexer.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

typedef enum {
    SOME_IDENTIFIER,
    SOME_KEYWORD,
    SOME_PREFIX,
    SOMETHING_ELSE
} TokenKind;

typedef struct {
    TokenKind kind;
    stb_lex_location location;
    char *string;
} Token;

typedef struct { char *key; Token value; } StringToKind;
StringToKind *lookup_table = NULL;

stb_lexer lexer = {0};

char *read_entire_file(const char *filename) {
    FILE *f = fopen(filename, "r");

    if(f == NULL) {
        fprintf(stderr, "Could not open file '%s': %s\n", filename, strerror(errno));
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;

    return string;
}

Token get_token() {
    StringToKind* result = shgetp_null(lookup_table, lexer.string);

    if(result != NULL) {
        return (Token) {
            .kind = result->value.kind,
            .string = strdup(lexer.string),
        };
    } else {
        stb_lex_location loc;
        stb_c_lexer_get_location(&lexer, lexer.where_firstchar, &loc);
        return (Token) {
            .kind = SOME_IDENTIFIER,
            .location = loc,
            .string = strdup(lexer.string)
        };
    }
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        fprintf(stderr, "ERROR: no filename given!\n");
        fprintf(stderr, "Usage ./main <filename>\n");
        return 1;
    }

    char *file_contents = read_entire_file(argv[1]);

    stb_c_lexer_init(
        &lexer,
        file_contents,
        NULL,
        malloc(sizeof(char)*4096),
        sizeof(char)*4096
    );

    shput(lookup_table, "alignas",       (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "alignof",       (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "auto",          (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "break",         (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "case",          (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "constexpr",     (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "continue",      (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "default",       (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "do",            (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "else",          (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "enum",          (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "extern",        (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "false",         (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "for",           (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "goto",          (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "if",            (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "inline",        (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "nullptr",       (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "restrict",      (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "return",        (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "sizeof",        (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "static",        (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "static_assert", (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "struct",        (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "switch",        (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "thread_local",  (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "true",          (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "typedef",       (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "typeof",        (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "typeof_unqual", (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "union",         (Token) { .kind = SOME_KEYWORD });
    shput(lookup_table, "while",         (Token) { .kind = SOME_KEYWORD });

    shput(lookup_table, "volatile",      (Token) { .kind = SOME_PREFIX  });
    shput(lookup_table, "register",      (Token) { .kind = SOME_PREFIX  });
    shput(lookup_table, "const",         (Token) { .kind = SOME_PREFIX  });
    shput(lookup_table, "signed",        (Token) { .kind = SOME_PREFIX  });
    shput(lookup_table, "unsigned",      (Token) { .kind = SOME_PREFIX  });

    Token lookback[2] = { 0 };

    unsigned int counter = 0;

    bool flushing = false;

    while(stb_c_lexer_get_token(&lexer)) {
        if (flushing) {
            char *start_pointer = lexer.where_firstchar;
            char *end_pointer   = start_pointer;
            while (lexer.token != ')') {
                end_pointer = lexer.where_lastchar;
                stb_c_lexer_get_token(&lexer);
            }
            printf("%.*s)\n", (int)(end_pointer - start_pointer) + 1, start_pointer);

            flushing = false;
        }

        switch(lexer.token) {
        case CLEX_parse_error: {
            fprintf(stderr, "parsing error!\n");
            return 1;
        } break;
        case CLEX_id: {
            lookback[0] = lookback[1];
            lookback[1] = get_token();

            if(lookback[0].kind == SOME_PREFIX && (lookback[1].kind == SOME_IDENTIFIER || lookback[1].kind == SOME_PREFIX)) {
                size_t original_size = strlen(lookback[1].string);
                size_t new_size = original_size + 2 + strlen(lookback[0].string);
                char *new_string = malloc(new_size);
                snprintf(new_string, new_size, "%s %s", lookback[0].string, lookback[1].string);
                free(lookback[1].string);
                lookback[1].string = new_string;
            }

        } break;
        case '(': {

            if(counter < 2) break;

            if(lookback[0].kind == SOME_IDENTIFIER && lookback[1].kind == SOME_IDENTIFIER) {
                printf(
                    "%s:%d:%d: %s %s(",
                    argv[1],
                    lookback[1].location.line_number,
                    lookback[1].location.line_offset,
                    lookback[0].string,
                    lookback[1].string
                );

                flushing = true;
            }

            lookback[0] = lookback[1];
            lookback[1] = (Token) { .kind = SOMETHING_ELSE };

        } break;
        case '*': {
            if(lookback[1].kind == SOME_IDENTIFIER) {
                size_t original_size = strlen(lookback[1].string);
                char *new_string = malloc(original_size + 2);
                snprintf(new_string, original_size + 2, "%s*", lookback[1].string);
                free(lookback[1].string);
                lookback[1].string = new_string;
            }
        } break;
        default:

            lookback[0] = lookback[1];
            lookback[1] = (Token) { .kind = SOMETHING_ELSE };
        }


        counter++;
    }

    return 0;
}
