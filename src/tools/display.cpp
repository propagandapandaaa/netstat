#include "../include/display.h"

/* Convert bytes to their correct orders of magnitude for formatting */
std::string formatDataRate(double bytes_per_sec)
{
    const char *units[] = {"B/s", "KB/s", "MB/s", "GB/s"};
    int unit = 0;
    double value = bytes_per_sec;

    while (value >= 1024 && unit < 3)
    {
        value /= 1024;
        unit++;
    }

    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << value << units[unit];
    return ss.str();
}

void display(std::vector<PairStats> pairs)
{
    /* Params for specifying column width */
    const int IPV6_MAX_LENGTH = 47;
    const int PROTO_WIDTH = 7;
    const int NUM_WIDTH = 10;

    std::stringstream header;
    header << std::left
           << std::setfill(' ')
           << std::setw(IPV6_MAX_LENGTH) << "src" << "  "
           << std::setw(IPV6_MAX_LENGTH) << "dst" << "  "
           << std::setw(PROTO_WIDTH) << "proto" << "  "
           << std::right
           << std::setw(NUM_WIDTH) << "bsent" << "  "
           << std::setw(NUM_WIDTH) << "brecv" << "  "
           << std::setw(NUM_WIDTH) << "psent" << "  "
           << std::setw(NUM_WIDTH) << "precv" << "  "
           << std::setw(NUM_WIDTH) << "btotal" << "  "
           << std::setw(NUM_WIDTH) << "ptotal" << "\n";

    std::string output = header.str();

    std::string separator(IPV6_MAX_LENGTH * 2 + PROTO_WIDTH + NUM_WIDTH * 6 + 24, '-');
    output += separator + "\n";

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
           << std::setw(NUM_WIDTH) << formatDataRate(bytes_sent_per_sec) << "  "
           << std::setw(NUM_WIDTH) << formatDataRate(bytes_recv_per_sec) << "  "
           << std::setw(NUM_WIDTH) << std::fixed << std::setprecision(1) << packets_sent_per_sec << "  "
           << std::setw(NUM_WIDTH) << std::fixed << std::setprecision(1) << packets_recv_per_sec << "  "
           << std::setw(NUM_WIDTH) << formatDataRate(static_cast<double>(pair.bytes_sent + pair.bytes_recv)) << "  "
           << std::setw(NUM_WIDTH) << std::fixed << std::setprecision(1)
           << static_cast<double>(pair.packets_sent + pair.packets_recv) << "\n";

        output += ss.str();
    }

    clear();
    printw("%s", output.c_str());
    refresh();
}
