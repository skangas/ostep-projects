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

size_t xfread(void *restrict ptr, size_t size, size_t nitems, FILE *restrict stream)
{
	size_t read = fread(ptr, size, nitems, stream);

	if (ferror(stream))
		die("Error reading file");

	return read;
}


bool extract_byte(FILE *fd)
{
	char ch;
	uint32_t count;
	size_t read;

	read = xfread(&count, 4, 1, fd);
	if (!read)
		return false;

	read = xfread(&ch, 1, 1, fd);
	if (!read)
		return false;

	for (uint32_t i = 0; i < count; i++)
		fwrite(&ch, 1, 1, stdout);

	return true;
}


int main(int argc, char **argv)
{
	if (argc == 1) {
		printf("wunzip: file1 [file2 ...]\n");
		return EXIT_FAILURE;
	}

	for (int i = 1; i < argc; i++) {
		FILE *fd = open_file(argv[i]);

		while (extract_byte(fd))
			;

		close_file(fd);
	}

	return EXIT_SUCCESS;
}
