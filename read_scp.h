#ifndef __READ_SCP_H
#define __READ_SCP_H

#include "easyecg.h"

int read_scp(const unsigned char *data, unsigned long file_size, struct easyecg_context *ctx);

#endif

