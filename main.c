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

int main(int cli_arg_count, char **cli_args) {
    char_buffer line = read_line();

    printf("%s\n", line);

    exit(0);
}

