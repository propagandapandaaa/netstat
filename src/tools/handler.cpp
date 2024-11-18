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

    void getProtocolInfo(const u_char *transport_header, uint8_t protocol_type,
                         u_int16_t &src_port, u_int16_t &dst_port)
    {
        switch (protocol_type)
        {
        case IPPROTO_TCP:
        {
            struct tcphdr *tcp_header = (struct tcphdr *)transport_header;
            src_port = ntohs(tcp_header->th_sport);
            dst_port = ntohs(tcp_header->th_dport);
            break;
        }
        case IPPROTO_UDP:
        {
            struct udphdr *udp_header = (struct udphdr *)transport_header;
            src_port = ntohs(udp_header->uh_sport);
            dst_port = ntohs(udp_header->uh_dport);
            break;
        }
        case IPPROTO_ICMP:
        {
            src_port = 0;
            dst_port = 0;
            break;
        }
        case IPPROTO_ICMPV6:
        {
            src_port = 0;
            dst_port = 0;
            break;
        }
        default:
        {
            src_port = 0;
            dst_port = 0;
            break;
        }
        }
    }

    std::string formatConnectionString(const char *src_ip, const char *dst_ip,
                                       u_int16_t src_port, u_int16_t dst_port,
                                       const std::string &protocol, bool is_ipv6)
    {
        char connection_string[128];

        std::string proto_lower = protocol;
        std::transform(proto_lower.begin(), proto_lower.end(), proto_lower.begin(),
                       [](unsigned char c)
                       { return std::tolower(c); });

        if (protocol == "icmp" || protocol == "icmpv6")
        {
            if (is_ipv6)
            {
                snprintf(connection_string, sizeof(connection_string), "[%s]_[%s]_%s",
                         src_ip, dst_ip, protocol.c_str());
            }
            else
            {
                snprintf(connection_string, sizeof(connection_string), "%s_%s_%s",
                         src_ip, dst_ip, protocol.c_str());
            }
        }
        else
        {
            if (is_ipv6)
            {
                snprintf(connection_string, sizeof(connection_string), "[%s]:%d_[%s]:%d_%s",
                         src_ip, src_port, dst_ip, dst_port, protocol.c_str());
            }
            else
            {
                snprintf(connection_string, sizeof(connection_string), "%s:%d_%s:%d_%s",
                         src_ip, src_port, dst_ip, dst_port, protocol.c_str());
            }
        }

        return std::string(connection_string);
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

        int ip_header_len = ip_header->ip_hl * 4;
        const u_char *transport_header = packet + sizeof(struct ether_header) + ip_header_len;
        getProtocolInfo(transport_header, ip_header->ip_p, src_port, dst_port);

        std::string connection_string = formatConnectionString(
            src_ip, dst_ip, src_port, dst_port, protocol, false);

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

        const u_char *transport_header = packet + sizeof(struct ether_header) + sizeof(struct ip6_hdr);
        getProtocolInfo(transport_header, ip6_header->ip6_nxt, src_port, dst_port);

        std::string connection_string = formatConnectionString(
            src_ip, dst_ip, src_port, dst_port, protocol, true);

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
