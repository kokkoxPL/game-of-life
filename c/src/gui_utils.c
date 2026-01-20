#include "gui_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void input_row(struct nk_context *ctx, int *variable, char *buffer,
               int *buffer_len, int change, const char *label) {
  if (*buffer_len == 0) {
    snprintf(buffer, 16, "%d", *variable);
    *buffer_len = (int)strlen(buffer);
  }

  nk_layout_row_begin(ctx, NK_DYNAMIC, 30, 6);
  {
    nk_layout_row_push(ctx, 0.125f);
    nk_spacing(ctx, 1);

    nk_layout_row_push(ctx, 0.4f);
    nk_labelf(ctx, NK_TEXT_CENTERED, label);

    nk_layout_row_push(ctx, 0.1f);
    if (nk_button_label(ctx, "-")) {
      int value = atoi(buffer);
      *variable = (value > 0) ? value - change : 0;
      snprintf(buffer, 16, "%d", *variable);
      *buffer_len = (int)strlen(buffer);
    }

    nk_layout_row_push(ctx, 0.15f);
    if (nk_edit_string(ctx, NK_EDIT_FIELD, buffer, buffer_len, 16,
                       nk_filter_decimal)) {
      int value = atoi(buffer);
      if (value < 0)
        value = 0;
      *variable = value;
    }

    nk_layout_row_push(ctx, 0.1f);
    if (nk_button_label(ctx, "+")) {
      *variable = atoi(buffer) + change;
      snprintf(buffer, 16, "%d", *variable);
      *buffer_len = (int)strlen(buffer);
    }

    nk_layout_row_push(ctx, 0.125f);
    nk_spacing(ctx, 1);
  }
  nk_layout_row_end(ctx);
}