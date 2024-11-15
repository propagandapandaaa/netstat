#include "../include/listener.h"

/* Reads packets from buffer and calls handler function */
void listener(const char *interface, std::unordered_map<std::string, PairData> &pairs, std::atomic<bool> &running)
{
    char errorBuffer[PCAP_ERRBUF_SIZE];
    pcap_t *handle;

    handle = pcap_open_live(interface, BUFSIZ, 1, 1000, errorBuffer);
    if (handle == nullptr)
    {
        std::cerr << "invalid network interface";
        exit(EXIT_FAILURE);
    }

    while (running)
    {
        //  (pcap_loop(handle, 0, packetHandler, reinterpret_cast<u_char *>(&pairs)) < 0)
        if (pcap_dispatch(handle, 1, packetHandler, reinterpret_cast<u_char *>(&pairs)) == -1)
        {
            std::cerr << "Error while capturing packets: " << pcap_geterr(handle) << std::endl;
            pcap_close(handle);
            exit(EXIT_FAILURE);
        }
    }

    pcap_close(handle);
}
