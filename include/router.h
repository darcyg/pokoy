#pragma once

#include <stdbool.h>

#define ROUTES_MAX 8

bool route_match(const char *matcher, const char *path);
