#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include "siglock.h"
#include "apperr.h"
#include "parse.h"
#include "learn.h"
#include "card.h"

static void help(FILE *fp);
static void pversion(FILE *fp);

main(int argc, char **argv)
{
	int sig[] = { SIGHUP, SIGINT, SIGTERM, 0}, ch, i;
	struct learnopt opt;
	char *envnow;
	time_t now;

	memset(&opt, 0, sizeof opt);
	siglock(SIGLOCK_INIT, sig);
	while ((ch = getopt(argc, argv, "ehv")) != -1)
		switch (ch) {
		case 'e':
			opt.exact = 1;
			break;
		case 'h':
			help(stdout);
			return 0;
		case 'v':
			pversion(stdout);
			return 0;
		case '?':
		default:
			help(stderr);
			return 1;
		}
	if ((argv += optind, argc -= optind) == 0) {
		help(stderr);
		return 1;
	}
	if ((envnow = getenv("HARDV_NOW"))) {
		if (parsetm(envnow, &now) == -1) {
			aeprint(envnow);
			return 1;
		}
	} else
		now = time(NULL);
	while (*argv) {
		if (learn(*argv, now, &opt) == -1) {
			if (inparse)
				fprintf(stderr,
					"%s, line %d: %s\n",
					*argv, lineno, aestr());
			else
				aeprint(*argv);
			return 1;
		}
		argv++;
	}
	return 0;
}

static void help(FILE *fp)
{
	fputs("hardv [options] file [file ...]\n", fp);
	fputs("\n", fp);
	fputs("options\n", fp);
	fputs("\n", fp);
	fputs("-e	enable exact quiz time\n", fp);
	fputs("-h	print this help information\n", fp);
	fputs("-v	print version and building information\n", fp);
}

static void pversion(FILE *fp)
{
	fprintf(fp, "hardv %s\n", VERSION);
	fprintf(fp, "%s\n", COPYRT);
	fputc('\n', fp);
	fprintf(fp, "NCARD:	%d\n", NCARD);
	fprintf(fp, "NFIELD:	%d\n", NFIELD);
	fprintf(fp, "NBLANK:	%d\n", NBLANK);
	fprintf(fp, "LINESZ:	%d\n", LINESZ);
	fprintf(fp, "KEYSZ:	%d\n", KEYSZ);
	fprintf(fp, "VALSZ:	%d\n", VALSZ);
}
