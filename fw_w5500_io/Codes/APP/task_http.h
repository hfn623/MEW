#ifndef TASK_HTTP_H
#define TASK_HTTP_H

#include "w5500.h"

extern wiz_NetInfo gWIZNETINFO;

void task_http_data(void *parm);
void task_http_conn(void *parm);

#endif
