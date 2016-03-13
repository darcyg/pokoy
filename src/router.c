#include <stdbool.h>
#include <stdio.h>
#include <regex.h>

#include "pokoy.h"

void route_add(
	struct router *r,
	const char *matcher,
	struct response (*route)()
) {
	r->routes[r->matchers_n] = route;
	r->matchers[r->matchers_n] = matcher;
	r->matchers_n++;
}

bool route_match(const char *matcher, const char *path)
{
	regex_t regex;

	int err = regcomp(&regex, path, REG_EXTENDED);
	if (err) {
		printf("Cannot compile regex: %s\n", matcher);
		return false;
	}

	int match = regexec(&regex, matcher, 0, NULL, 0);

	regfree(&regex);

	return match != REG_NOMATCH;
}
