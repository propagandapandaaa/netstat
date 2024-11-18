/*
Author
Name: Daniel Jacobs
Login: xjacob00
*/

#include "../include/display.h"

namespace
{
    /* Convert bytes to their correct orders of magnitude for formatting */
    std::string formatBytes(double bytes)
    {
        const char *units[] = {"", "K", "M", "G"};
        int unit = 0;
        double value = bytes;

        while (value >= 1024 && unit < 3)
        {
            value /= 1024;
            unit++;
        }

        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << value << units[unit];
        return ss.str();
    }

    /* Converts packets to orders of magnitude */
    std::string formatPackets(double packets)
    {
        const char *units[] = {"", "k", "M", "B"};
        int unit = 0;
        double value = packets;

        while (value >= 1000 && unit < 3)
        {
            value /= 1000;
            unit++;
        }

        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << value;
        if (unit > 0)
        {
            ss << units[unit];
        }
        return ss.str();
    }
}

void display(std::vector<PairStats> pairs)
{
    /* Params for specifying column width */
    const int IPV6_MAX_LENGTH = 47;
    const int PROTO_WIDTH = 7;
    const int NUM_WIDTH = 5;

    std::stringstream header;
    header << std::left
           << std::setfill(' ')
           << std::setw(IPV6_MAX_LENGTH) << "Src IP:port" << "  "
           << std::setw(IPV6_MAX_LENGTH) << "Dst IP:port" << "  "
           << std::setw(PROTO_WIDTH) << "Proto" << "  "
           << std::right
           << std::setw(NUM_WIDTH * 2 + 2) << "         Rx" << "  "
           << std::setw(NUM_WIDTH * 2 + 2) << "               Tx" << "\n";
    std::string output = header.str();

    std::stringstream units;
    units << std::left
          << std::setfill(' ')
          << std::setw(IPV6_MAX_LENGTH) << " "
          << std::setw(IPV6_MAX_LENGTH) << " "
          << std::setw(PROTO_WIDTH) << " "
          << std::right
          << std::setw(NUM_WIDTH * 2 + 2) << "              b/s p/s" << "  "
          << std::setw(NUM_WIDTH * 2 + 2) << "          b/s p/s" << "\n";

    output += units.str();

    std::string separator(IPV6_MAX_LENGTH * 2 + PROTO_WIDTH + NUM_WIDTH * 4 + 24, '-');
    output += separator + "\n";

    /* This is to prevent displaying more pairs than there are */
    int limit = std::min(10, static_cast<int>(pairs.size()));
    for (int i = 0; i < limit; ++i)
    {
        const auto &pair = pairs[i];
        int seconds = pair.seconds;

        /* Rates per second */
        double bytes_sent_per_sec = static_cast<double>(pair.bytes_sent) / seconds;
        double bytes_recv_per_sec = static_cast<double>(pair.bytes_recv) / seconds;
        double packets_sent_per_sec = static_cast<double>(pair.packets_sent) / seconds;
        double packets_recv_per_sec = static_cast<double>(pair.packets_recv) / seconds;

        std::stringstream ss;
        ss << std::left
           << std::setfill(' ')
           << std::setw(IPV6_MAX_LENGTH) << pair.src_ip << "  "
           << std::setw(IPV6_MAX_LENGTH) << pair.dst_ip << "  "
           << std::setw(PROTO_WIDTH) << pair.proto << "  "
           << std::right
           << std::setw(NUM_WIDTH) << " "
           << std::setw(NUM_WIDTH) << formatBytes(bytes_recv_per_sec) << " "
           << std::setw(NUM_WIDTH + 2) << formatPackets(packets_recv_per_sec) << "     "

           << std::setw(NUM_WIDTH) << formatBytes(bytes_sent_per_sec) << " "
           << std::setw(NUM_WIDTH + 2) << formatPackets(packets_sent_per_sec) << "\n";
        output += ss.str();
    }

    clear();
    printw("%s", output.c_str());
    refresh();
}
