#pragma once
#include "chip.h"

void icemu_ui_init();
void icemu_ui_deinit();
void icemu_ui_add_element(char *title, Chip *chip);
int icemu_ui_refresh();
