#include "../include/listener.h"

/* Reads packets from buffer and calls handler function */
void listener(const char *interface, std::unordered_map<std::string, PairData> &pairs, std::atomic<bool> &running)
{
    char errorBuffer[PCAP_ERRBUF_SIZE];
    pcap_t *handle;

    std::cout << interface << std::endl;
    handle = pcap_open_live(interface, BUFSIZ, 1, 1000, errorBuffer);

    if (handle == nullptr)
    {
        std::string error = "Invalid network interface";
        if (errorBuffer[0] != '\0')
        {
            error += ": " + std::string(errorBuffer);
        }
        throw std::runtime_error(error);
    }

    int linktype = pcap_datalink(handle);

    int packets;
    if (linktype == LOOPBACK)
    {
        while (running)
        {
            //  (pcap_loop(handle, 0, packetHandler, reinterpret_cast<u_char *>(&pairs)) < 0)
            packets = pcap_dispatch(handle, 1000, processLoopbackPacket, reinterpret_cast<u_char *>(&pairs));
            if (packets == -1)
            {
                std::cerr << "Error while capturing packets: " << pcap_geterr(handle) << std::endl;
                pcap_close(handle);
                exit(EXIT_FAILURE);
            }
        }
    }

    else
    {
        while (running)
        {
            packets = pcap_dispatch(handle, 1000, packetHandler, reinterpret_cast<u_char *>(&pairs));
            if (packets == -1)
            {
                std::cerr << "Error while capturing packets: " << pcap_geterr(handle) << std::endl;
                pcap_close(handle);
                exit(EXIT_FAILURE);
            }
        }
    }

    pcap_close(handle);
}
