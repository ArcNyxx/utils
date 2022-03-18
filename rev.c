/* rev - reverse characterwise
 * Copyright (C) 2022 ArcNyxx
 * see LICENCE file for licensing information */

#include <stddef.h>
#include <stdio.h>
#include <string.h>

static void rev(FILE *fp);

static void
rev(FILE *fp)
{
	char *line = NULL;
	size_t size = 0, utfc;
	ssize_t nread = 0, chars;
	while ((nread = getline(&line, &size, fp)) > 0) {
		chars = nread - (line[nread - 1] == '\n'), utfc = 0;

		while (--chars != -1)
			/* check for continued UTF-8 glyph */
			if ((line[chars] & 0xc0) == 0x80) {
				++utfc;
			} else {
				fwrite(line + chars, 1, utfc + 1, stdout);
				utfc = 0;
			}

		if (utfc != 0) /* handle unflushed UTF-8 glyph */
			fwrite(line, 1, utfc, stdout);
		if (line[nread - 1] == '\n') /* handle final line of file */
			putchar('\n');
	}
	fclose(fp);
}

int
main(int argc, char **argv)
{
	FILE *fp;
	if (argc == 1)
		rev(stdin);
	for (argv = &argv[1]; *argv != NULL; argv = &argv[1])
		if (!strcmp(*argv, "-"))
			rev(stdin);
		else if ((fp = fopen(*argv, "r")) != NULL)
			rev(fp);
		else /* opening file failed */
			fprintf(stderr, "rev: unable to open %s\n", *argv);
	fflush(stdout);
}
