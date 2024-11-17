#ifndef OPTIONS_H
#define OPTIONS_H

#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdexcept>

/* Structure that holds the options */
typedef struct options
{
    std::string interface;
    std::string order;
} options;

/* Function for parsing command line arguments */
void parseArgs(int argc, char **argv, options *options);

#endif