#include <stdbool.h>

#include "pokoy.h"

void route_add(struct router *r, const char *matcher, int (*route)())
{
	r->routes[0] = route;
	r->matchers[0] = matcher;
}

bool route_match(const char *matcher, const char *path)
{
	if (matcher && path) {
		return true;
	}

	return false;
}
