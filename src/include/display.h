#ifndef DISPLAY_H
#define DISPLAY_H

#include <ncurses.h>
#include "handler.h"
#include <vector>

void display(std::vector<std::pair<std::string, PairData>> pairs);

#endif