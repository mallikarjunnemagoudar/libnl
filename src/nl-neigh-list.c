/*
 * src/nl-neigh-list.c      List Neighbours
 *
 *	This library is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Lesser General Public
 *	License as published by the Free Software Foundation version 2.1
 *	of the License.
 *
 * Copyright (c) 2003-2008 Thomas Graf <tgraf@suug.ch>
 */

#include "neigh-utils.h"

static void print_usage(void)
{
	printf(
	"Usage: nl-neigh-list [OPTION]... [NEIGHBOUR]\n"
	"\n"
	"Options\n"
	" -f, --format=TYPE     Output format { brief | details | stats }\n"
	" -h, --help            Show this help\n"
	" -v, --version         Show versioning information\n"
	"\n"
	"Neighbour Options\n"
	" -a, --addr=ADDR       Destination address of neighbour\n"
	" -l, --lladdr=ADDR     Link layer address of neighbour\n"
	" -d, --dev=DEV         Device the neighbour is connected to\n"
	"     --family=FAMILY   Destination address family\n"
	"     --state=STATE     Neighbour state, (default = permanent)\n"
	);
	exit(0);
}

int main(int argc, char *argv[])
{
	struct nl_sock *sock;
	struct rtnl_neigh *neigh;
	struct nl_cache *link_cache, *neigh_cache;
	struct nl_dump_params params = {
		.dp_type = NL_DUMP_ONELINE,
		.dp_fd = stdout,
	};
 
	sock = nlt_alloc_socket();
	nlt_connect(sock, NETLINK_ROUTE);
	link_cache = nlt_alloc_link_cache(sock);
	neigh_cache = nlt_alloc_neigh_cache(sock);
 	neigh = nlt_alloc_neigh();
 
	for (;;) {
		int c, optidx = 0;
		enum {
			ARG_FAMILY = 257,
			ARG_STATE = 258,
		};
		static struct option long_opts[] = {
			{ "format", 1, 0, 'f' },
			{ "help", 0, 0, 'h' },
			{ "version", 0, 0, 'v' },
			{ "addr", 1, 0, 'a' },
			{ "lladdr", 1, 0, 'l' },
			{ "dev", 1, 0, 'd' },
			{ "family", 1, 0, ARG_FAMILY },
			{ "state", 1, 0, ARG_STATE },
			{ 0, 0, 0, 0 }
		};
	
		c = getopt_long(argc, argv, "f:hva:l:d:", long_opts, &optidx);
		if (c == -1)
			break;

		switch (c) {
		case 'f': params.dp_type = nlt_parse_dumptype(optarg); break;
		case 'h': print_usage(); break;
		case 'v': nlt_print_version(); break;
		case 'a': parse_dst(neigh, optarg); break;
		case 'l': parse_lladdr(neigh, optarg); break;
		case 'd': parse_dev(neigh, link_cache, optarg); break;
		case ARG_FAMILY: parse_family(neigh, optarg); break;
		case ARG_STATE: parse_state(neigh, optarg); break;
		}
 	}

	nl_cache_dump_filter(neigh_cache, &params, OBJ_CAST(neigh));

	return 0;
}