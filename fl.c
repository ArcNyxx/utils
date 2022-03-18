/* fl - format line length
 * Copyright (C) 2022 ArcNyxx
 * see LICENCE file for licensing information */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void fl(FILE *fp);

static size_t ll = 79;

static void
fl(FILE *fp)
{
	char *line = NULL;
	size_t size = 0, count = 0, han;
	ssize_t nread = 0;
	while ((nread = getline(&line, &size, fp)) > 0) {
		han = 0;

		/* output leading spaces on new paragraph */
		if (nread == 1) {
			do {
				fwrite("\n\n", 1, 2, stdout);
				count = 0;
			} while((nread = getline(&line, &size, fp)) == 1);

			for (han = 0; line[han] == ' '; ++han)
				++count;
			fwrite(line, 1, han, stdout);
		}

		for (;;) {
			size_t len = 0, tcount = 0;
			while (line[han] == ' ')
				++han;
			if (line[han] == '\n' || line[han] == '\0')
				break; /* current line empty, get new */
			for (; line[han + len] != ' ' && line[han + len] !=
					'\n' && line[han + len] != '\0'; ++len)
				tcount += (line[han + len] & 0xc0) != 0x80;

			if (count != 0) { /* prevent extra space and newline */
				if (count + tcount >= ll) {
					putchar('\n');
					count = 0;
				} else {
					putchar(' ');
					++count;
				}
			}

			fwrite(line + han, 1, len, stdout);
			han += len, count += tcount;
		}
	}
}

int
main(int argc, char **argv)
{
	if (argv[1] != NULL && argv[2] != NULL && !strcmp(argv[1], "-w"))
		if ((ll = strtoull(argv[2], NULL, 10)) == 0) {
			fprintf(stderr, "fl: invalid width %s\n", argv[2]);
			return 1;
		} else {
			argc -= 2, argv = &argv[2]; /* realign argv */
		}

	FILE *fp;
	if (argc == 1)
		fl(stdin);
	for (argv = &argv[1]; *argv != NULL; argv = &argv[1])
		if (!strcmp(*argv, "-"))
			fl(stdin);
		else if ((fp = fopen(*argv, "r")) != NULL)
			fl(fp);
		else /* opening file failed */
			fprintf(stderr, "fl: unable to open %s\n", *argv);
	fflush(stdout);
}
