#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

/* A growable buffer with text in it. */
typedef char *char_buffer;

/* Indices of a substring stored in some other buffer. */
struct string_offset {
    int start;
    int count;
};

typedef char_buffer *string_buffer;

char_buffer read_line(void) {
    char_buffer result = NULL;
    int read_count = 0;
    bool done = false;

    /* Read from stdin, likely blocking until the user presses enter/return,
       and return the result as a single buffer, with no trailing newline
       character. */
    while (true) {
        int prev_len = arrlen(result);

        const int segment_size = 80;
        char *segment = arraddnptr(result, segment_size);

        int added_count;
        /* Get text from stdin, up to and including a single newline character;
           this may return null if a file or pipe is being used as stdin, and
           there are no characters left in the file. */
        if (fgets(segment, segment_size, stdin)) {
            added_count = strlen(segment);
        } else {
            added_count = 0;
        }

        arrsetlen(result, prev_len + added_count);

        /* Test if we got a newline character, which is how we tell that we
           actually have the whole line. */
        if (feof(stdin) || arrlast(result) == '\n') break;
    }

    /* Make sure we remove the newline from the returned result, since we just
       want the contents of that line. */
    if (arrlen(result) > 0 && arrlast(result) == '\n') {
        arrpop(result);
    }

    /* Add a null character to the end, just in case the user wants to treat
       the line as a c-string, for other standard library functions. */
    arrpush(result, '\0');
    /* 'pop' it, so that arrlen(result) == strlen(result). It is up to the user
       whether they rely on the null character or not. Basically any array
       operation applied to result could invalidate it as a c-string. */
    arrpop(result);

    return result;
}

string_buffer split_words(char_buffer line) {
    int line_len = arrlen(line);

    string_buffer result = NULL;
    char_buffer next = NULL;

    for (int i = 0; i < line_len; i++) {
        char c = line[i];
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\0') {
            if (arrlen(next) > 0) {
                arrpush(next, '\0');
                arrpop(next);

                arrpush(result, next);
                next = NULL;
            }
        } else {
            arrpush(next, c);
        }
    }

    if (arrlen(next) > 0) {
        arrpush(next, '\0');
        arrpop(next);

        arrpush(result, next);
        next = NULL;
    }

    return result;
}

bool compare_charbuff_cstr(char_buffer buff, char *cstr) {
    int cstr_len = strlen(cstr);

    if (cstr_len != arrlen(buff)) return false;
    /* else */
    return strncmp(buff, cstr, cstr_len) == 0;
}

bool match_keyword(
    string_buffer *words,
    char *keyword
) {
    if (compare_charbuff_cstr((*words)[0], keyword)) {
        arrdel(*words, 0);
        return true;
    }

    return false;
}

bool match_or_explain_keyword(
    string_buffer *words,
    char *keyword,
    char *help_message,
    bool help
) {
    if (help) {
        printf("%s\n", help_message);
    } else if (compare_charbuff_cstr((*words)[0], keyword)) {
        arrdel(*words, 0);
        return true;
    }

    return false;
}

int main(int cli_arg_count, char **cli_args) {
    while (true) {
        printf(">");
        char_buffer line = read_line();

        string_buffer words = split_words(line);

        if (arrlen(words) == 0) continue;

        bool help = match_keyword(&words, "help");

        if (match_or_explain_keyword(
            &words,
            "echo",
            "echo: Prints input back to the screen.",
            help
        )) {
            for (int i = 0; i < arrlen(words); i++) {
                if (i > 0) printf(" ");
                printf("%s", words[i]);
            }
            printf("\n");
        } else if (match_or_explain_keyword(
            &words,
            "exit",
            "exit: Stop taking input and close the program.",
            help
        )) {
            exit(0);
        } else if (!help) {
            printf("Unknown command '%s'. Type 'help' for a list of "
                "commands.\n", words[0]);
        }
    }
}

