/* $Id: capinfo.c,v 1.2 2002/08/08 03:35:15 mattbing Exp $ */

#include "config.h"

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "libpcap.h"
#include "snoop.h"

void print_pcap(struct pcap_info *);
void usage();

int
main(int argc, char *argv[])
{
	struct pcap_info p;
	int i, fd, flag;

	if (argc == 0)
		usage();

	for (i = 1; i < argc; i++) {
		flag = 0;

		(void)printf("%s:\n", argv[i]);
		if ((fd = open(argv[i], O_RDONLY, 0)) < 0) {
			warn("could not open");
			continue;
		}

		if (is_pcap(fd)) {
			stat_pcap(fd, &p);
			flag = 1;
			printf("%s pcap file\n", argv[1]);
			print_pcap(&p);
		}

		/* rewind */
		if (lseek(fd, 0, SEEK_SET) != 0)
			err(1, "lseek");

		if (is_snoop(fd)) {
			printf("%s snoop file\n", argv[1]);
			stat_snoop(fd);
			flag = 1;
		}

		if (flag == 0)
			warnx("unknown format");

		(void)printf("\n");
	}

	return 0;
}

void
print_pcap(struct pcap_info *p)
{
	char *start, *finish;

	printf("\tpcap (%s%s)\n", 
		(p->modified ? "modified, ": ""), p->swapped);

	(void)printf("\tversion: %d.%d\n", p->phdr.version_major, 
		p->phdr.version_minor);
	(void)printf("\tzone: %d\n", p->phdr.thiszone);
	(void)printf("\tsig figs: %d\n", p->phdr.sigfigs);
	(void)printf("\tsnaplen: %d\n", p->phdr.snaplen);

	(void)printf("\tlinktype: %s\n", p->linktype);
	(void)printf("\t%d packets, %d bytes\n", p->cnt, p->bytes);
	if (p->trunc > 0)
		(void)printf("\t%d packets truncated (larger than snaplen)\n", 
			p->trunc);

	if (p->cnt > 0) {
		start = ctime(&p->start_tm.tv_sec);
		(void)printf("\tfirst packet: %s", start);
		finish = ctime(&p->finish_tm.tv_sec);
		(void)printf("\tlast  packet: %s", finish);
	}

}

void
usage()
{
	(void)fprintf(stderr, "capinfo <files>\n");
	exit(1);
}
