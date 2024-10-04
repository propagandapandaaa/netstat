#include "../include/display.h"

void display(std::vector<std::pair<std::string, PairData>> pairs)
{

    int limit = std::min(10, static_cast<int>(pairs.size()));

    std::string output;

    for (int i = 0; i < limit; ++i)
    {
        const auto &[key, data] = pairs[i];
        output += "Key: " + key +
                  " | Bytes: " + std::to_string(data.bytes) +
                  " | Packets: " + std::to_string(data.packets) + "\n";
    }

    // Use printw to display the output string
    clear();
    printw("%s", output.c_str());
    refresh();
}