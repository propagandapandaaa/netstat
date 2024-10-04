#include "../include/stats.h"

/*
TODO:
Match SRC:DST and DST:SRC pairs
Count total bytes/s and packets/s
get top 10
display
*/
namespace
{
    std::string splitIp(std::string)
    {
    }

    /* FOR TESTING ONLY */
    void printAllPairs(const std::unordered_map<std::string, PairData> &pairs)
    {
        std::cout << "Key                | Bytes         | Packages\n";
        std::cout << "-----------------------------------------------\n";
        for (const auto &pair : pairs)
        {
            std::cout << pair.first << " | "
                      << pair.second.bytes << " | "
                      << pair.second.packets << '\n';
        }
    }

    /* Convert const char params to enum to avoid using strcmp() */
    OrderBy setOrder(const char *order)
    {
        if (strcmp(order, "p") == 0)
        {
            return packets;
        }
        else
        {
            return bytes;
        }
    }

    /* Sorts pairs by packet count or bytes, set by the order param */
    std::vector<std::pair<std::string, PairData>> orderPairs(std::unordered_map<std::string, struct PairData> &pairs, OrderBy order)
    {
        std::vector<std::pair<std::string, PairData>> orderedPairs(pairs.begin(), pairs.end());

        std::sort(orderedPairs.rbegin(), orderedPairs.rend(),
                  [order](const std::pair<std::string, PairData> &a, const std::pair<std::string, PairData> &b)
                  {
                      if (order == bytes)
                      {
                          return a.second.bytes < b.second.bytes;
                      }
                      else
                      {
                          return a.second.packets < b.second.packets;
                      }
                  });

        return orderedPairs;
    }
}

/* Timer function, gets packets once per second and processes them */
void getStats(const char *orderString, std::unordered_map<std::string, struct PairData> &pairs)
{

    std::unordered_map<std::string, struct PairData> pairsCopy;

    /* Wait for the first batch of packets */
    std::this_thread::sleep_for(std::chrono::seconds(1));

    OrderBy order = setOrder(orderString);

    while (true)
    {
        // printAllPairs(pairs);
        // std::cout << " looping " << std::endl;
        const auto start = std::chrono::high_resolution_clock::now();

        /* indented block so that lock_guard goes out of scope */
        {
            std::lock_guard<std::mutex> lock(pair_lock);
            pairsCopy = pairs;

            /* reset statistics */
            pairs.clear();
        }

        /* Vector of sorted pairs */
        std::vector<std::pair<std::string, PairData>> orderedPairs = orderPairs(pairsCopy, order);

        display(orderedPairs);

        /* Accounts for processing time of the packets */
        const auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double, std::milli> elapsed = end - start;

        auto sleepDuration = std::chrono::milliseconds(1000) - elapsed;

        if (sleepDuration > std::chrono::milliseconds(0))
        {
            std::this_thread::sleep_for(sleepDuration);
        }
        else
        {
            /*  FIGURE OUT WHAT TO DO WITH THIS
                even though it probably will never reach it */
            continue;
        }
    }
}