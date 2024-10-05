#include "../include/display.h"

/*

std::string src_ip;
    std::string dst_ip;
    std::string proto;

    int bytes_sent;
    int bytes_recv;

    int packets_sent;
    int patckets_recv;

    int bytes_total;   // Sum of sent and received bytes
    int packets_total; // Sum of sent and receivet packets

    */
void display(std::vector<PairStats> pairs)
{

    int limit = std::min(10, static_cast<int>(pairs.size()));

    std::string output = "src                 |dst                  | proto | bsent  | brecv | psent | precv | btotal | ptotal\n";

    // std::cout << "src                 |dst                  | proto | bsent  | brecv | psent | precv | btotal | ptotal" << std::endl;
    for (int i = 0; i < limit; ++i)
    {
        // const auto &[src_ip, dst_ip, proto, bytes_sent, bytes_recv, packets_sent, patckets_recv, bytes_total, packets_total] = pairs[i];
        const auto pair = pairs[i];
        output += pair.src_ip + "   " +
                  pair.dst_ip + "       " +
                  pair.proto + "      " +
                  std::to_string(pair.bytes_sent) + "      " +
                  std::to_string(pair.bytes_recv) + "      " +
                  std::to_string(pair.packets_sent) + "      " +
                  std::to_string(pair.patckets_recv) + "\n";

        /*
        output += "Key: " + key +
                  " | Bytes: " + std::to_string(data.bytes) +
                  " | Packets: " + std::to_string(data.packets) + "\n";
        */
    }

    // Use printw to display the output string
    clear();
    printw("%s", output.c_str());
    refresh();
}
