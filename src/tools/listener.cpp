#include "../include/listener.h"

/* Reads packets from buffer and calls handler function */
void listener(const char *interface, std::unordered_map<std::string, PairData> &pairs, std::atomic<bool> &running)
{
    char errorBuffer[PCAP_ERRBUF_SIZE];
    // pcap_t *handle;

    // Add before pcap_open_live
    pcap_t *handle = pcap_create(interface, errorBuffer);
    if (handle == NULL)
    {
        throw std::runtime_error(errorBuffer);
    }

    // Set larger buffer size (e.g. 32MB)
    if (pcap_set_buffer_size(handle, 32 * 1024 * 1024) != 0)
    {
        pcap_close(handle);
        throw std::runtime_error("Failed to set buffer size");
    }

    // Set immediate mode for faster processing
    if (pcap_set_immediate_mode(handle, 1) != 0)
    {
        pcap_close(handle);
        throw std::runtime_error("Failed to set immediate mode");
    }

    // Activate the handle
    if (pcap_activate(handle) != 0)
    {
        pcap_close(handle);
        throw std::runtime_error(pcap_geterr(handle));
    }

    handle = pcap_open_live(interface, BUFSIZ, 1, 1000, errorBuffer);

    if (handle == nullptr)
    {
        std::string error = "Network interface";
        if (errorBuffer[0] != '\0')
        {
            error += " - " + std::string(errorBuffer);
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
