#ifndef STATS_H
#define STATS_H

#include "listener.h"
#include "display.h"
#include <chrono>
#include <map>
#include <vector>
#include <algorithm>

enum OrderBy
{
    bytes,
    packets
};

void getStats(const char *orderString, std::unordered_map<std::string, PairData> &pairs, std::atomic<bool> &running);

#endif
