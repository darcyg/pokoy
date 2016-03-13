#pragma once

#include <inttypes.h>

#include "router.h"

struct server {
	uint16_t port_no;
	struct router *router;
};

struct router {
	int (*routes[ROUTES_MAX])();
	const char *matchers[ROUTES_MAX];
};

void route_add(struct router *r, const char *matcher, int(*route)());
void server_run();
