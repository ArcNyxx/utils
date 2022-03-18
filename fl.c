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
	size_t size = 0, count = 0, han = 0;
	ssize_t nread = 0;
	while ((nread = getline(&line, &size, fp)) > 0) {
		han = 0;

		/* output leading spaces on new paragraph */
		if (nread == 1) {
			fwrite("\n\n", 1, 2, stdout);
			nread = getline(&line, &size, fp), count = 0;

			for (han = 0; line[han] == ' '; ++han)
				++count;
			fwrite(line, 1, han, stdout);
		}

		for (;;) {
			size_t len = 0, tcount = 0;
			while (line[han] == ' ')
				++han;
			if (line[han] == '\n' || line[han] == '\0')
				break; /* current line empty */
			for (; line[han + len] != ' ' && line[han + len] !=
					'\n' && line[han + len] != '\0'; ++len)
				tcount += (line[han + len] & 0xc0) != 0x80;

			if (count + tcount >= ll) {
				fputc('\n', stdout);
				count = 0;
			} else if (count != 0) { /* prevent extra on para */
				fputc(' ', stdout);
				++count;
			}

			fwrite(line + han, 1, len, stdout);
			han += len, count += tcount;
		}
	}
}

int
main(int argc, char **argv)
{
	if (!strncmp(argv[1], "--width=", 8))
		if ((ll = strtoull(&argv[1][8], NULL, 10)) == 0) {
			fprintf(stderr, "fl: invalid width %s\n", &argv[1][8]);
			return 1;
		} else {
			argc -= 1, argv = &argv[1]; /* realign argv */
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
