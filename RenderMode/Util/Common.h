#pragma once
#include <cstdio>
#define DBG_ASSERT(condition, msg, ...) do {if (!(condition)) {fprintf(stderr, "%s:%d ERROR: %s\n" msg, #condition, __LINE__, __FILE__, __VA_ARGS__ );}} while (0)