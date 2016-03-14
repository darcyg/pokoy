#pragma once

#include <inttypes.h>

struct request {
	char method[7 + 1];
	char path[2083 + 1];
};

struct response {
	uint16_t code;
	const char *status;
	const char *body;
};

int socket_init(const char *port);
struct request req_parse(int client);
void res_send(int client, struct response *res);
