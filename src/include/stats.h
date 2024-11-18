/*
Author
Name: Daniel Jacobs
Login: xjacob00
*/

#ifndef STATS_H
#define STATS_H

#include "listener.h"
#include "display.h"
#include <chrono>
#include <map>
#include <vector>
#include <algorithm>
#include <stdexcept>

/* Holds the order set by the user */
enum OrderBy
{
    bytes,
    packets
};

/* Main function for handling the processing and priting of network statistics */
void getStats(const char *orderString, std::unordered_map<std::string, PairData> &pairs, std::atomic<bool> &running);

#endif
