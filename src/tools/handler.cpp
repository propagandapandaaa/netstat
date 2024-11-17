#include "../include/handler.h"

namespace
{
    std::string getProtocol(const u_int8_t protocol)
    {
        switch (protocol)
        {
        case IPPROTO_TCP:
            return "tcp";
        case IPPROTO_UDP:
            return "udp";
        case IPPROTO_ICMP:
            return "icmp";
        case IPPROTO_ICMPV6:
            return "icmpv6";
        default:
            return "unknown";
        }
    }

    void updatePairStats(std::unordered_map<std::string, PairData> *pairs,
                         const std::string &connection_string,
                         const uint32_t packet_length)
    {
        const std::lock_guard<std::mutex> lock(pair_lock);
        auto it = pairs->find(connection_string);
        if (it != pairs->end())
        {
            it->second.packets++;
            it->second.bytes += packet_length;
        }
        else
        {
            PairData new_data;
            new_data.packets = 1;
            new_data.bytes = packet_length;
            pairs->emplace(connection_string, new_data);
        }
    }
}

/*  USES: lock_guard(pair_lock)
    Adds new packets to pairs hashmap, if pair exists, bytes and packet count is incremented */
void packetHandler(u_char *userData, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
    auto *pairs = reinterpret_cast<std::unordered_map<std::string, PairData> *>(userData);
    struct ether_header *eth_header = (struct ether_header *)packet;

    struct tcphdr *packet_header; // FIX SO THAT IT SUPPORTS OTHER PROTOCOLS

    char src_ip[INET6_ADDRSTRLEN];
    char dst_ip[INET6_ADDRSTRLEN];
    char connection_string[128];

    std::string protocol;

    u_int16_t src_port, dst_port;

    switch (ntohs(eth_header->ether_type))
    {
    /* IPV4 */
    case ETH_P_IP:
    {

        struct ip *ip_header = (struct ip *)(packet + sizeof(struct ether_header));
        inet_ntop(AF_INET, &(ip_header->ip_src), src_ip, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ip_header->ip_dst), dst_ip, INET_ADDRSTRLEN);

        packet_header = (struct tcphdr *)(packet + sizeof(struct ether_header) + sizeof(struct ip));

        src_port = ntohs(packet_header->th_sport);
        dst_port = ntohs(packet_header->th_dport);

        protocol = getProtocol(ip_header->ip_p);

        snprintf(connection_string, sizeof(connection_string), "%s:%d_%s:%d_%s",
                 src_ip, src_port, dst_ip, dst_port, protocol.c_str());
        updatePairStats(pairs, connection_string, pkthdr->len);
        break;
    }

    /* IPV6 */
    case ETH_P_IPV6:
    {
        struct ip6_hdr *ip6_header = (struct ip6_hdr *)(packet + sizeof(struct ether_header));
        inet_ntop(AF_INET6, &(ip6_header->ip6_src), src_ip, INET6_ADDRSTRLEN);
        inet_ntop(AF_INET6, &(ip6_header->ip6_dst), dst_ip, INET6_ADDRSTRLEN);

        packet_header = (struct tcphdr *)(packet + sizeof(struct ether_header) + sizeof(struct ip6_hdr));
        src_port = ntohs(packet_header->th_sport);
        dst_port = ntohs(packet_header->th_dport);

        protocol = getProtocol(ip6_header->ip6_nxt);

        snprintf(connection_string, sizeof(connection_string), "[%s]:%d_[%s]:%d_%s",
                 src_ip, src_port, dst_ip, dst_port, protocol.c_str());
        updatePairStats(pairs, connection_string, pkthdr->len);
        break;
    }
    default:
    {
        /* Unsupported type, skip */
        break;
    }
    }
}

void processLoopbackPacket(u_char *userData, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
    auto *pairs = reinterpret_cast<std::unordered_map<std::string, PairData> *>(userData);

    /* skip ETH header, loopback doesnt need */
    const u_char *ip_packet = packet + 4;

    char src_ip[INET6_ADDRSTRLEN];
    char dst_ip[INET6_ADDRSTRLEN];
    char connection_string[128];
    std::string protocol;
    u_int16_t src_port, dst_port;
    struct tcphdr *packet_header;

    uint8_t ip_version = (*ip_packet >> 4) & 0xf;

    switch (ip_version)
    {
    case 4: // IPv4
    {
        struct ip *ip_header = (struct ip *)ip_packet;
        inet_ntop(AF_INET, &(ip_header->ip_src), src_ip, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ip_header->ip_dst), dst_ip, INET_ADDRSTRLEN);

        packet_header = (struct tcphdr *)(ip_packet + sizeof(struct ip));
        src_port = ntohs(packet_header->th_sport);
        dst_port = ntohs(packet_header->th_dport);
        protocol = getProtocol(ip_header->ip_p);

        snprintf(connection_string, sizeof(connection_string), "%s:%d_%s:%d_%s",
                 src_ip, src_port, dst_ip, dst_port, protocol.c_str());
        updatePairStats(pairs, connection_string, pkthdr->len);
        break;
    }
    case 6: // IPv6
    {
        struct ip6_hdr *ip6_header = (struct ip6_hdr *)ip_packet;
        inet_ntop(AF_INET6, &(ip6_header->ip6_src), src_ip, INET6_ADDRSTRLEN);
        inet_ntop(AF_INET6, &(ip6_header->ip6_dst), dst_ip, INET6_ADDRSTRLEN);

        packet_header = (struct tcphdr *)(ip_packet + sizeof(struct ip6_hdr));
        src_port = ntohs(packet_header->th_sport);
        dst_port = ntohs(packet_header->th_dport);
        protocol = getProtocol(ip6_header->ip6_nxt);

        snprintf(connection_string, sizeof(connection_string), "[%s]:%d_[%s]:%d_%s",
                 src_ip, src_port, dst_ip, dst_port, protocol.c_str());
        updatePairStats(pairs, connection_string, pkthdr->len);
        break;
    }
    default:
    {
        /* Unsupported type, skip */
        break;
    }
    }
}
