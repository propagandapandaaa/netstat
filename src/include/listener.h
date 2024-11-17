#ifndef LISTENER_H
#define LISTENER_H

#include <iostream>
#include <cstring>
#include <mutex>
#include <thread>
#include <atomic>
#include <stdexcept>
#include "handler.h"

#define LOOPBACK 0
#define ETH 1

void listener(const char *interface, std::unordered_map<std::string, PairData> &pairs, std::atomic<bool> &running);

#endif