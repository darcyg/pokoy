#pragma once

#include <inttypes.h>

struct request {
	char method[7 + 1];
	char path[2083 + 1];
	struct query_pair *query_pairs;
	size_t query_pairs_n;
};

struct response {
	uint16_t code;
	const char *status;
	const char *body;
};

struct query_pair {
	char key[64];
	char value[64];
};

int socket_init(const char *port);
void req_parse(int client, struct request *req);
void res_send(int client, struct response *res);
