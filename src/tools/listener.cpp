#include "../include/listener.h"

/* Reads packets from buffer and calls handler function */
void listener(const char *interface, Mutex_Hashmap<std::string, struct PairData> &pairs)
{
    char errorBuffer[PCAP_ERRBUF_SIZE];
    pcap_t *handle;

    handle = pcap_open_live(interface, BUFSIZ, 1, 1000, errorBuffer);
    if (handle == nullptr)
    {
        std::cerr << "invalid network interface";
        exit(EXIT_FAILURE);
    }

    std::cout << "interface: " << interface << std::endl;

    if (pcap_loop(handle, 0, packetHandler, reinterpret_cast<u_char *>(&pairs)) < 0)
    {
        std::cerr << "Error while capturing packets: " << pcap_geterr(handle) << std::endl;
        pcap_close(handle);
        exit(EXIT_FAILURE);
    }

    pcap_close(handle);
}
