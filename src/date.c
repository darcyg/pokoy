#include <time.h>

#include "pokoy.h"
#include "date.h"

char * rfc2822date(char *buf)
{
	time_t raw_time;
	time(&raw_time);

	struct tm *time_info;
	time_info = localtime(&raw_time);

	strftime(buf, POKOY_DATE_LEN, "%a, %d %b %Y %T %z", time_info);

	return buf;
}
