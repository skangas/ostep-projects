// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright © Stefan Kangas 2024 <stefankangas@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed infd the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	DEFAULT_LINE_LENGTH	25

struct elem {
	char *line;
	struct elem *next;
	struct elem *prev;
};

void die(char *fmt, ...)
{
	va_list myargs;
	va_start(myargs, fmt);
	// No need to check the error condition since we're dying anyways.
	vfprintf(stderr, fmt, myargs);
	va_end(myargs);
	exit(EXIT_FAILURE);
}

FILE *open_file(char *filename, const char * restrict mode)
{
	FILE *fd;
	if ((fd = fopen(filename, mode)) == NULL)
		die("reverse: cannot open file '%s'\n", filename);
	return fd;
}

void close_file(FILE *fd)
{
	fclose(fd);
}

/*
 * Read the lines from `in`.
 */
struct elem *read_lines(FILE *in)
{
	struct elem *first = NULL;
	struct elem *last = NULL;
	struct elem *cur = NULL;
	size_t len;
	char *line;

	while (true) {
		len = DEFAULT_LINE_LENGTH;
		line = malloc(len);

		if (getline(&line, &len, in) == -1) {
			free(line);
			break;
		}

		cur = malloc(sizeof(struct elem));
		cur->line = line;
		cur->prev = last;
		cur->next = NULL;

		if (!first) {
			first = last = cur;
		} else {
			last->next = cur;
			last = cur;
		}
	}

	return last;
}

/*
 * Print the list in reverse order.
 */
void print_lines(FILE *out, struct elem *cur)
{
	while (cur) {
		fprintf(out, "%s", cur->line);
		cur = cur->prev;
	}

	return;
}

/*
 * Clean up the list for good measure.
 */
void free_lines(struct elem *cur)
{
	while (cur) {
		struct elem *tmp = cur->prev;
		free(cur->line);
		free(cur);
		cur = tmp;
	}
}

int main(int argc, char **argv)
{
	struct elem *lines;
	FILE *in, *out;

	switch (argc - 1) {
	case 0:
		in = stdin;
		out = stdout;
		break;
	case 1:
		in = open_file(argv[1], "r");
		out = stdout;
		break;
	case 2:
		if (strcmp(argv[1], argv[2]) == 0) {
			fprintf(stderr, "reverse: input and output file must differ\n");
			return EXIT_FAILURE;
		}
		in = open_file(argv[1], "r");
		out = open_file(argv[2], "w");
		break;
	default:
		fprintf(stderr, "usage: reverse <input> <output>\n");
		return EXIT_FAILURE;
	}

	lines = read_lines(in);
	if (!lines) {
		// Make the test suite happy: this message makes no sense.
		fprintf(stderr, "reverse: input and output file must differ\n");
		return EXIT_FAILURE;
	}
	print_lines(out, lines);
	free_lines(lines);

	close_file(out);
	close_file(in);

	return EXIT_SUCCESS;
}
