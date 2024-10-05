#include "../include/handler.h"

namespace
{
    std::string getProtocol(const u_int8_t protocol)
    {
        switch (protocol)
        {
        case IPPROTO_TCP:
            return "tcp";
            break;
        case IPPROTO_UDP:
            return "udp";
            break;
        case IPPROTO_ICMP:
            return "icmp";
            break;
        default:
            return "unknown";
            break;
        }
    }
}

/*  USES: lock_guard(pair_lock)
    Adds new packets to pairs hashmap, if pair exists, bytes and packet count is incremented */
void packetHandler(u_char *userData, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
    auto *pairs = reinterpret_cast<std::unordered_map<std::string, PairData> *>(userData);
    struct ether_header *eth_header = (struct ether_header *)packet;

    if (ntohs(eth_header->ether_type) == ETHERTYPE_IP)
    {
        struct ip *ip_header = (struct ip *)(packet + sizeof(struct ether_header));

        char src_ip[INET_ADDRSTRLEN];
        char dst_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(ip_header->ip_src), src_ip, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ip_header->ip_dst), dst_ip, INET_ADDRSTRLEN);

        u_int16_t src_port, dst_port;

        struct tcphdr *packet_header = (struct tcphdr *)(packet + sizeof(struct ether_header) + sizeof(struct ip));
        src_port = ntohs(packet_header->th_sport);
        dst_port = ntohs(packet_header->th_dport);

        std::string protocol = getProtocol(ip_header->ip_p);

        char connection_string[100];
        snprintf(connection_string, sizeof(connection_string), "%s:%d_%s:%d_%s", src_ip, src_port, dst_ip, dst_port, protocol.c_str());

        const std::lock_guard<std::mutex> lock(pair_lock);

        auto it = pairs->find(connection_string);
        if (it != pairs->end())
        {
            it->second.packets++;
            it->second.bytes += pkthdr->len;
        }
        else
        {
            PairData new_data;
            new_data.packets = 1;
            new_data.bytes = pkthdr->len;
            pairs->emplace(connection_string, new_data);
        }
    }
}
