#pragma once

#include <inttypes.h>

#include "router.h"
#include "http.h"

struct server {
	uint16_t port_no;
	struct router *router;
};

void route_add(
	struct router *r,
	const char *matcher,
	struct response (*route)()
);
void server_run();
