#ifndef LISTENER_H
#define LISTENER_H

#include <iostream>
#include <cstring>
#include <mutex>
#include "handler.h"

void listener(const char *interface, Mutex_Hashmap<std::string, struct PairData> &pairs);

#endif