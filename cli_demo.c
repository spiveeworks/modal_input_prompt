#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#include "imcli.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

int main(int cli_arg_count, char **cli_args) {
    while (true) {
        string_buffer words = prompt(">");

        bool help = match_keyword(&words, "help", NULL);

        bool any_matched = false;

        if (match_or_explain_keyword_detailed(
            &words,
            "echo",
            "echo: Prints input back to the screen.\n",

            "Usage: echo [argument] [...]\n"
            "Print arguments to the screen. Words with multiple spaces or tabs\n"
            "between them will be printed with a single space between them instead.\n",
            help,
            &any_matched
        )) {
            for (int i = 0; i < arrlen(words); i++) {
                if (i > 0) printf(" ");
                printf("%s", words[i]);
            }
            printf("\n");
        }

        if (match_or_explain_keyword(
            &words,
            "multiple word test",
            "multiple word test: Dummy command to test keyword parsing.\n",
            help,
            &any_matched
        )) {
            printf(
                "Multiple word test was run with %d arguments.\n",
                (int)arrlen(words)
            );
        }

        if (match_or_explain_keyword_simple(
            &words,
            "exit",
            "exit: Stop taking input and close the program.\n",
            help,
            &any_matched
        )) {
            exit(0);
        }

        /* This will never return true, since the only line that will trigger
           it is `help help`. We just want help to have a help message. */
        match_or_explain_keyword_detailed(
            &words,
            "help",
            "help: Lists commands and explains their usage.\n",

            "Usage: help [command]\n"
            "Print a detailed message about how to use the given command. If no command\n"
            "is specified, then a summary of all available commands is given instead.\n",
            help,
            &any_matched
        );

        if (!any_matched && arrlen(words) > 0) {
            printf("Unknown command '%s'. Type 'help' for a list of "
                "commands.\n", words[0]);
        }

        sbfree(&words);
    }
}

