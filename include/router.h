#pragma once

#include <stdbool.h>

#define ROUTES_MAX 8

struct router {
	struct response (*routes[ROUTES_MAX])();
	const char *matchers[ROUTES_MAX];
	uint8_t matchers_n;
};

bool route_match(const char *matcher, const char *path);
