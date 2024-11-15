#include "../include/display.h"

void display(std::vector<PairStats> pairs)
{
    // Define column widths
    const int IPV6_MAX_LENGTH = 46; // INET6_ADDRSTRLEN
    const int PROTO_WIDTH = 7;
    const int NUM_WIDTH = 7;

    // Create format string for each column with proper spacing
    std::stringstream header;
    header << std::left
           << std::setfill(' ')
           << std::setw(IPV6_MAX_LENGTH) << "src" << " | "
           << std::setw(IPV6_MAX_LENGTH) << "dst" << " | "
           << std::setw(PROTO_WIDTH) << "proto" << " | "
           << std::right
           << std::setw(NUM_WIDTH) << "bsent" << " | "
           << std::setw(NUM_WIDTH) << "brecv" << " | "
           << std::setw(NUM_WIDTH) << "psent" << " | "
           << std::setw(NUM_WIDTH) << "precv" << " | "
           << std::setw(NUM_WIDTH) << "btotal" << " | "
           << std::setw(NUM_WIDTH) << "ptotal" << "\n";

    std::string output = header.str();

    std::string separator(IPV6_MAX_LENGTH * 2 + PROTO_WIDTH + NUM_WIDTH * 6 + 24, '-');
    output += separator + "\n";

    int limit = std::min(10, static_cast<int>(pairs.size()));
    for (int i = 0; i < limit; ++i)
    {
        const auto &pair = pairs[i];
        std::stringstream ss;
        ss << std::left
           << std::setfill(' ')
           << std::setw(IPV6_MAX_LENGTH) << pair.src_ip << " | "
           << std::setw(IPV6_MAX_LENGTH) << pair.dst_ip << " | "
           << std::setw(PROTO_WIDTH) << pair.proto << " | "
           << std::right
           << std::setw(NUM_WIDTH) << pair.bytes_sent << " | "
           << std::setw(NUM_WIDTH) << pair.bytes_recv << " | "
           << std::setw(NUM_WIDTH) << pair.packets_sent << " | "
           << std::setw(NUM_WIDTH) << pair.packets_recv << " | "
           << std::setw(NUM_WIDTH) << (pair.bytes_sent + pair.bytes_recv) << " | "
           << std::setw(NUM_WIDTH) << (pair.packets_sent + pair.packets_recv) << "\n";

        output += ss.str();
    }

    /* Display output */
    clear();
    printw("%s", output.c_str());
    refresh();
}
