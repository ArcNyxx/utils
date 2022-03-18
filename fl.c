/* fl - format line length
 * Copyright (C) 2022 ArcNyxx
 * see LICENCE file for licensing information */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct wl_output {
	size_t len, count;
};

static void fl(FILE *fp);
static struct wl_output wl(const char *str, size_t *restrict handled);

static size_t ll = 79;

static void
fl(FILE *fp)
{
	char *line = NULL;
	size_t size = 0, count = 0, handled = 0;
	ssize_t nread = 0;
	for (; (nread = getline(&line, &size, fp)) > 0; handled = 0) {
		/* output leading spaces on new paragraph */
		if (nread == 1) {
			fwrite("\n\n", 1, 2, stdout);
			nread = getline(&line, &size, fp), count = 0;

			for (handled = 0; line[handled] == ' '; ++handled)
				++count;
			fwrite(line, 1, handled, stdout);
		}

		struct wl_output ret;
		for (;;) {
			ret = wl(line, &handled);
			if (ret.count == 0)
				break; /* current line empty */

			if (count + ret.count >= ll) {
				fputc('\n', stdout);
				count = 0;
			} else if (count != 0) { /* prevent extra on para */
				fputc(' ', stdout);
				++count;
			}

			fwrite(line + handled, 1, ret.len, stdout);
			handled += ret.len, count += ret.count;
		}
	}
}

static struct wl_output
wl(const char *str, size_t *restrict handled)
{
	size_t len = 0, count = 0;
	while ((str[*handled] == ' ' || str[*handled] == '\n') &&
			str[*handled] != '\0')
		++*handled; /* ignore leading space */
	while (str[*handled + len] != ' ' && str[*handled + len] != '\n' &&
			str[*handled + len] != '\0')
		count += (str[*handled + len] & 0xc0) != 0x80, ++len;
	struct wl_output output = { len, count };
	return output;
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
