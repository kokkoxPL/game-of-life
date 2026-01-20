#ifndef GUI_UTILS_H
#define GUI_UTILS_H

#include "nuklear_config.h"

#include "nuklear.h"

void input_row(struct nk_context *ctx, int *variable, char *buffer,
               int *buffer_len, int change, const char *label);

#endif