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
#include <stdlib.h>

#define	LINE_MAX	2048

void die(char *str)
{
	// Avoid `perror` to make the OSTEP test suite happy.
	/* perror(str); */
	printf("%s\n", str);
	exit(EXIT_FAILURE);
}

void print_file(char *file)
{
	FILE* fd;
	char line[LINE_MAX];

	if ((fd = fopen(file, "r")) == NULL)
		die("wcat: cannot open file");

	while (fgets(line, LINE_MAX, fd) != NULL)
		printf("%s", line);

	if (ferror(fd))
		die("Error reading file");

	if (fclose(fd) == EOF)
		die("Unable to close file");
}

int main(int argc, char **argv)
{
	if (argc == 1)
		return EXIT_SUCCESS;

	for (int i = 1; i < argc; i++)
		print_file(argv[i]);

	return EXIT_SUCCESS;
}
