/*
 * uncompress.c - uncompress a compressed file
 *
 * Copyright (c) 2010 Sascha Hauer <s.hauer@pengutronix.de>, Pengutronix
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <common.h>
#include <command.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <fs.h>
#include <uncompress.h>

static int do_uncompress(int argc, char *argv[])
{
	unsigned long seek = 0;
	int from, to, ret;
	int argc_min;
	int opt;

	while ((opt = getopt(argc, argv, "s:")) > 0) {
		switch (opt) {
		case 's':
			seek = strtoull_suffix(optarg, NULL, 0);
			break;
		}
	}

	argc_min = optind + 2;

	if (argc < argc_min)
		return COMMAND_ERROR_USAGE;

	from = open(argv[optind], O_RDONLY);
	if (from < 0) {
		perror("open");
		return 1;
	}

	to = open(argv[optind + 1], O_WRONLY | O_CREAT);
	if (to < 0) {
		perror("open");
		ret = 1;
		goto exit_close;
	}

	lseek(to, seek, SEEK_SET);

	ret = uncompress_fd_to_fd(from, to, uncompress_err_stdout);

	if (ret)
		printf("failed to decompress\n");

	close(to);
exit_close:
	close(from);
	return ret;
}

BAREBOX_CMD_HELP_START(uncompress)
BAREBOX_CMD_HELP_TEXT("Uncompress a compressed INFILE to OUTFILE.")
BAREBOX_CMD_HELP_TEXT("")
BAREBOX_CMD_HELP_TEXT("Options:")
BAREBOX_CMD_HELP_OPT ("-s COUNT", "seek to COUNT bytes from the start of OUTFILE")
BAREBOX_CMD_HELP_END

BAREBOX_CMD_START(uncompress)
	.cmd            = do_uncompress,
	BAREBOX_CMD_DESC("uncompress a compressed file")
	BAREBOX_CMD_OPTS("[-s COUNT] INFILE OUTFILE")
	BAREBOX_CMD_GROUP(CMD_GRP_FILE)
BAREBOX_CMD_END
