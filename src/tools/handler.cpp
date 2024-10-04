#include "../include/handler.h"

/* FOR TESTING PURPOSES ONLY */
void printThreePairs(const std::unordered_map<std::string, PairData> &pairs)
{
    int count = 0;

    for (const auto &pair : pairs)
    {
        std::cout << "Connection: " << pair.first << " | Packet Count: " << pair.second.packets << std::endl;
        count++;
        if (count >= 3)
        {
            break;
        }
    }

    if (pairs.size() < 3)
    {
        std::cout << "There are fewer than 3 connections in the map." << std::endl;
    }
}

/* Adds new packets to pairs hashmap, if pair exists, bytes and packet count is incremented */
void packetHandler(u_char *userData, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
    auto *pairs = reinterpret_cast<Mutex_Hashmap<std::string, struct PairData> *>(userData);
    std::cout << "Packet captured: length " << pkthdr->len << std::endl;
    struct ether_header *eth_header = (struct ether_header *)packet;

    if (ntohs(eth_header->ether_type) == ETHERTYPE_IP)
    {
        struct ip *ip_header = (struct ip *)(packet + sizeof(struct ether_header));

        char src_ip[INET_ADDRSTRLEN];
        char dst_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(ip_header->ip_src), src_ip, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ip_header->ip_dst), dst_ip, INET_ADDRSTRLEN);

        u_int16_t src_port, dst_port;
        if (ip_header->ip_p == IPPROTO_TCP)
        {
            struct tcphdr *tcp_header = (struct tcphdr *)(packet + sizeof(struct ether_header) + sizeof(struct ip));
            src_port = ntohs(tcp_header->th_sport);
            dst_port = ntohs(tcp_header->th_dport);
        }
        else if (ip_header->ip_p == IPPROTO_UDP)
        {
            struct udphdr *udp_header = (struct udphdr *)(packet + sizeof(struct ether_header) + sizeof(struct ip));
            src_port = ntohs(udp_header->uh_sport);
            dst_port = ntohs(udp_header->uh_dport);
        }
        else
        {
            return;
        }

        char connection_string[100];
        snprintf(connection_string, sizeof(connection_string), "%s:%d_%s:%d", src_ip, src_port, dst_ip, dst_port);

        auto it = pairs->find(connection_string);
        if (it != pairs->end())
        {
            it->second.packets++;
        }
        else
        {
            PairData new_data;
            new_data.packets = 1;
            pairs->emplace(connection_string, new_data);
        }
        printf("Connection: %s\n", connection_string);

        printThreePairs(*pairs);
    }
}
