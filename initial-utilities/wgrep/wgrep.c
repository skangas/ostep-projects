// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright © Stefan Kangas 2024 <stefankangas@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define	LINE_MAX	1024

void die(char *str)
{
	// Avoid `perror` to make the OSTEP test suite happy.
	/* perror(str); */
	printf("%s\n", str);
	exit(EXIT_FAILURE);
}

bool line_matches(char *needle, char *line, size_t len)
{
	return strnstr(line, needle, len) != NULL;
}

void print_matches(FILE *fd, char *needle)
{
	char *line = malloc(LINE_MAX * sizeof(char));
	bool always_match = false;
	size_t linecap = LINE_MAX;

	if (strlen(needle) == 0)
		always_match = true;

	while ((getline(&line, &linecap, fd)) > 0)
		if (always_match || line_matches(needle, line, linecap))
			printf("%s", line);

	free(line);
}

void print_matches_file(char *file, char *needle)
{
	FILE* fd;

	if ((fd = fopen(file, "r")) == NULL)
		die("wgrep: cannot open file");

	print_matches(fd, needle);

	if (ferror(fd))
		die("Error reading file");

	if (fclose(fd) == EOF)
		die("Unable to close file");
}

int main(int argc, char **argv)
{
	if (argc == 1) {
		printf("wgrep: searchterm [file ...]\n");
		return EXIT_FAILURE;
	}

	char *needle = argv[1];

	if (argc > 2)
		for (int i = 2; i < argc; i++)
			print_matches_file(argv[i], needle);
	else
		print_matches(stdin, needle);

	return EXIT_SUCCESS;
}
