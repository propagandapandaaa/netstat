/*
Author
Name: Daniel Jacobs
Login: xjacob00
*/

#include "../include/listener.h"

/* Reads packets from buffer and calls handler function */
void listener(const char *interface, std::unordered_map<std::string, PairData> &pairs, std::atomic<bool> &running)
{
    char errorBuffer[PCAP_ERRBUF_SIZE];
    pcap_t *handle;

    handle = pcap_create(interface, errorBuffer);
    if (handle == NULL)
    {
        throw std::runtime_error(errorBuffer);
    }

    if (pcap_set_buffer_size(handle, 32 * 1024 * 1024) != 0)
    {
        pcap_close(handle);
        throw std::runtime_error("Failed to set buffer size");
    }

    if (pcap_set_immediate_mode(handle, 1) != 0)
    {
        pcap_close(handle);
        throw std::runtime_error("Failed to set immediate mode");
    }

    if (pcap_activate(handle) != 0)
    {
        std::string error = pcap_geterr(handle);
        pcap_close(handle);
        throw std::runtime_error("Failed to activate: " + error);
    }

    int linktype = pcap_datalink(handle);
    bool is_loopback = (linktype == DLT_NULL);

    ListenerData listenerData = {
        .pairs = &pairs,
        .is_loopback = is_loopback};

    int packets;

    while (running)
    {
        packets = pcap_dispatch(handle, 1000, packetHandler, reinterpret_cast<u_char *>(&listenerData));
        if (packets == -1)
        {
            std::cerr << "Error while capturing packets: " << pcap_geterr(handle) << std::endl;
            pcap_close(handle);
            exit(EXIT_FAILURE);
        }
    }

    pcap_close(handle);
}
