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
#include <stdint.h>
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

FILE* open_file(char *file)
{
	FILE* fd;

	if ((fd = fopen(file, "r")) == NULL)
		die("wgrep: cannot open file");

	if (ferror(fd))
		die("Error reading file");

	return fd;
}

void close_file(FILE *fd)
{
	if (fclose(fd) == EOF)
		die("Unable to close file");
}

void print_one(char *prev, uint32_t *count)
{
	fwrite(count, 4, 1, stdout);
	fwrite(prev, 1, 1, stdout);
}

bool handle_byte(char *prev, uint32_t *count, FILE *fd)
{
	char this;
	size_t read;

	read = fread(&this, 1, 1, fd);
	if (ferror(fd))
		die("Error reading file");
	if (!read)
		return false;

	if (*count == 0) {
		// True only when program firsts start.
		*prev = this;
		(*count)++;
	} else if (*prev == this) {
		(*count)++;
	} else {
		print_one(prev, count);
		*prev = this;
		*count = 1;
	}

	return true;
}


int main(int argc, char **argv)
{
	if (argc == 1) {
		printf("wzip: file1 [file2 ...]\n");
		return EXIT_FAILURE;
	}

	char prev;
	uint32_t count = 0;

	for (int i = 1; i < argc; i++) {
		FILE *fd = open_file(argv[i]);

		while (handle_byte(&prev, &count, fd))
			;

		close_file(fd);
		
	}

	// Print last seen character.
	print_one(&prev, &count);

	return EXIT_SUCCESS;
}
