#ifndef HANDLER_H
#define HANDLER_H

// #include "hashmap.h"
#include "locks.h"

#include <unordered_map>
#include <pcap.h>
#include <cstdlib>
#include <iostream>
#include <pcap.h>

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <net/ethernet.h>
#include <string.h>

struct PairData
{
    int packets;
    int bytes;
};

void packetHandler(u_char *userData, const pcap_pkthdr *pkthdr, const u_char *packet);

#endif