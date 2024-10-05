#include "../include/display.h"

void display(std::vector<PairStats> pairs)
{
    int limit = std::min(10, static_cast<int>(pairs.size()));

    std::string output = "src                 |dst                  | proto | bsent  | brecv | psent | precv | btotal | ptotal\n";
    for (int i = 0; i < limit; ++i)
    {
        const auto pair = pairs[i];
        output += pair.src_ip + "   " +
                  pair.dst_ip + "       " +
                  pair.proto + "      " +
                  std::to_string(pair.bytes_sent) + "      " +
                  std::to_string(pair.bytes_recv) + "      " +
                  std::to_string(pair.packets_sent) + "      " +
                  std::to_string(pair.patckets_recv) + "\n";
    }

    /* Display output */
    clear();
    printw("%s", output.c_str());
    refresh();
}
