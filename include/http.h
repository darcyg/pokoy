#pragma once

#include <inttypes.h>

struct request {
	const char *method;
	const char *path;
};

struct response {
	uint8_t code;
	const char *status;
	const char *body;
};

int socket_init(uint16_t port);
struct request req_parse(int client);
void res_send(int client, struct response *res);
