#include "../include/display.h"

void display(std::vector<std::pair<std::string, PairData>> pairs)
{
    int limit = std::min(10, static_cast<int>(pairs.size()));

    for (int i = 0; i < limit; ++i)
    {
        const auto &[key, data] = pairs[i];
        std::cout << "Key: " << key
                  << " | Bytes: " << data.bytes
                  << " | Packages: " << data.packets
                  << '\n';
    }
}