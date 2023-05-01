#ifndef IMCLI_H
#define IMCLI_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
    char *keyword,
    bool *any_matched_out
) {
    bool any_matched = any_matched_out ? *any_matched_out : false;

    if (any_matched) return false;

    if (arrlen(*words) == 0) return false;

    bool this_matched = compare_charbuff_cstr((*words)[0], keyword);
    if (!this_matched) return false;

    /* else matched */
    if (any_matched_out) *any_matched_out = true;

    arrdel(*words, 0);
    return true;
}

bool match_or_explain_keyword_detailed(
    string_buffer *words,
    char *keyword,
    char *help_message,
    char *detailed_help_message,
    bool help,
    bool *any_matched_out
) {
    bool any_matched = any_matched_out ? *any_matched_out : false;
    /* print all basic help messages when a command like `help` was written by
       itself. */
    if (help && arrlen(*words) == 0 && !any_matched) {
        printf("%s", help_message);
        return false;
    }
    /* otherwise, we have to actually check if this command is the one that was
       written, and either display detailed help, or run the command. */
    if (match_keyword(words, keyword, any_matched_out)) {
        if (help) {
            printf("%s", detailed_help_message);
            return false;
        } else {
            return true;
        }
    }

    return false;
}

bool match_or_explain_keyword(
    string_buffer *words,
    char *keyword,
    char *help_message,
    bool help,
    bool *any_matched_out
) {
    return match_or_explain_keyword_detailed(
        words,
        keyword,
        help_message,
        help_message,
        help,
        any_matched_out
    );
}

bool match_or_explain_keyword_simple(
    string_buffer *words,
    char *keyword,
    char *help_message,
    bool help,
    bool *any_matched_out
) {
    if (!match_or_explain_keyword(
        words,
        keyword,
        help_message,
        help,
        any_matched_out
    )) {
        return false;
    }
    /* else it did match. */
    if (arrlen(*words) > 0) {
        printf("'%s' does not take any arguments.\n", keyword);
        return false;
    }
    /* else there are no arguments */
    return true;
}

#endif
