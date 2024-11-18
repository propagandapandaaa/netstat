/*
Author
Name: Daniel Jacobs
Login: xjacob00
*/

#ifndef DISPLAY_H
#define DISPLAY_H

#include <ncurses.h>
#include "handler.h"
#include "stats.h"
#include <iomanip>
#include <sstream>
#include <vector>

/* Structure for passing data from stats to display */
struct PairStats
{
    std::string src_ip;
    std::string dst_ip;
    std::string proto;

    int bytes_sent;
    int bytes_recv;

    int packets_sent;
    int packets_recv;

    int bytes_total;   // Sum of sent and received bytes
    int packets_total; // Sum of sent and receivet packets

    int seconds;
};

void display(std::vector<PairStats> pairs);

#endif