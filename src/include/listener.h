#ifndef LISTENER_H
#define LISTENER_H

#include <iostream>
#include <cstring>
#include <mutex>
#include <thread>
#include <atomic>
#include "handler.h"

void listener(const char *interface, std::unordered_map<std::string, PairData> &pairs, std::atomic<bool> &running);

#endif