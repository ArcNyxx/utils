/* jf - copy input to output
 * Copyright (C) 2022 ArcNyxx
 * see LICENCE file for licensing information */

#include <stdio.h>

int
main(int argc, char **argv)
{
	char buf[BUFSIZ];
	size_t numr, numw;

	do {
		if ((numr = fread(buf, sizeof(char), BUFSIZ, stdin)) == -1) {
			fputs("jf: unable to read stdin\n", stderr);
			return 1;
		}
		if ((numw = fwrite(buf, sizeof(char), numr, stdout)) != numr) {
			fputs("jf: unable to write stdout\n", stderr);
			return 1;
		}
	} while (numr == BUFSIZ);
	if (buf[numr - 1] != '\n')
		putchar('\n');

	for (argv = &argv[1]; argv != NULL; argv = &argv[1]) {
		puts(*argv);
	}
	fflush(stdout);
}
