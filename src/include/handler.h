/*
Author
Name: Daniel Jacobs
Login: xjacob00
*/

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
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#include <string.h>
#include <stdexcept>
#include <algorithm>

#define ETH_P_IP 0x0800   // IPv4 protocol
#define ETH_P_IPV6 0x86DD // IPv6 protocol

struct PairData
{
    int packets;
    int bytes;
    int timeout; // if this expires, pair gets deleted
    bool updated;
    int seconds; // amount of seconds in the system to calculate data flow per second
};

/* Structure for passing data from the listener to the handler */
struct ListenerData
{
    std::unordered_map<std::string, PairData> *pairs;
    bool is_loopback;
};

void packetHandler(u_char *userData, const pcap_pkthdr *pkthdr, const u_char *packet);

#endif