#include "../include/stats.h"

/*
TODO:
PROTOCOL needs to be a a part of the unique KEY in the HASHMAP :)))
then split the key again and retreive protocol for printing

Match SRC:DST and DST:SRC pairs
Count total bytes/s and packets/s
get top 10
display

*/

namespace
{
    /*  Input:  hashmap key
        Output: src_ip string */
    std::string getSrcIp(std::string key)
    {
        size_t index = key.find('_');
        if (index != std::string::npos)
        {
            return key.substr(0, index);
        }
        else
        {
            return "unknown";
        }
    }

    /*  Input:  hashmap key
        Output: dst_ip string */
    std::string getDstIp(std::string key)
    {
        size_t start_index = key.find('_');
        size_t end_index = key.find('_', start_index + 1);

        if (start_index != std::string::npos && end_index != std::string::npos)
        {
            return key.substr(start_index + 1, end_index - start_index - 1);
        }
        else
        {
            return "unknown";
        }
    }

    /*  Input:  hashmap key
        Output: protocol string */
    std::string getProtocol(std::string key)
    {
        size_t start_index = key.find('_');
        size_t end_index = key.find('_', start_index + 1);

        if (start_index != std::string::npos && end_index != std::string::npos)
        {
            return key.substr(end_index + 1);
        }
        else
        {
            return "unknown";
        }
    }

    std::vector<PairStats> formatData(std::unordered_map<std::string, struct PairData> &pairs)
    {
        std::vector<PairStats> result_data;
        for (const auto &pair : pairs)
        {
            PairStats tmp;
            tmp.src_ip = getSrcIp(pair.first);
            tmp.dst_ip = getDstIp(pair.first);
            tmp.proto = getProtocol(pair.first);
            bool found = false;

            if (!result_data.empty())
            {
                for (auto &data : result_data)
                {
                    /* Protocol matches */
                    if (tmp.proto == data.proto)
                    {
                        /* Current tmp src_ip is the sender */
                        if (tmp.src_ip == data.src_ip && tmp.dst_ip == data.dst_ip)
                        {
                            data.bytes_sent += pair.second.bytes;
                            data.packets_sent += pair.second.packets;

                            data.bytes_total += pair.second.bytes;
                            data.packets_total += pair.second.packets;
                            found = true;
                        }
                        /* Current tmp src_ip is the receiver */
                        if (tmp.src_ip == data.dst_ip && tmp.dst_ip == data.src_ip)
                        {
                            data.bytes_recv += pair.second.bytes;
                            data.patckets_recv += pair.second.packets;

                            data.bytes_total += pair.second.bytes;
                            data.packets_total += pair.second.packets;
                            found = true;
                        }
                        if (found)
                        {
                            break;
                        }
                    }
                }
            }

            /* The src:dst pair is new */
            if (!found)
            {
                tmp.bytes_sent = pair.second.bytes;
                tmp.bytes_recv = 0;

                tmp.packets_sent = pair.second.packets;
                tmp.patckets_recv = 0;

                tmp.bytes_total = pair.second.bytes;
                tmp.packets_total = pair.second.packets;

                result_data.emplace_back(tmp);
            }
        }
        return result_data;
    }

    /* FOR TESTING ONLY */
    void testPrintPairs(const std::unordered_map<std::string, PairData> &pairs)
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

    /* FOR TESTING ONLY */
    void testPrintStats(std::vector<PairStats> pairs)
    {
        std::cout << "src         | dst       | proto | bsent | brecv |psent | precv | btotal |ptotal \n";
        std::cout << "--------------------------------\n";
        for (const auto &pair : pairs)
        {
            std::cout << pair.src_ip << " | "
                      << pair.dst_ip << " | "
                      << pair.proto << " | "
                      << pair.bytes_sent << " | "
                      << pair.bytes_recv << " | "
                      << pair.packets_sent << " | "
                      << pair.patckets_recv << " | "
                      << pair.bytes_total << " | "
                      << pair.packets_total << '\n';
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
    std::vector<PairStats> orderData(std::vector<PairStats> &pairs, OrderBy order)
    {
        // std::vector<PairStats> orderedStats;
        std::sort(pairs.rbegin(), pairs.rend(),
                  [order](const PairStats &a, const PairStats &b)
                  {
                      if (order == bytes)
                      {
                          return a.bytes_total < b.bytes_total;
                      }
                      else
                      {
                          return a.packets_total < b.packets_total;
                      }
                  });
        return pairs;
    }

    /* DEPRECATED */
    std::vector<std::pair<std::string, PairData>> orderPairs(std::unordered_map<std::string, PairData> &pairs, OrderBy order)
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
void getStats(const char *orderString, std::unordered_map<std::string, PairData> &pairs)
{

    std::unordered_map<std::string, PairData> pairs_copy;

    /* Wait for the first batch of packets */
    std::this_thread::sleep_for(std::chrono::seconds(1));

    OrderBy order = setOrder(orderString);

    while (true)
    {
        const auto start = std::chrono::high_resolution_clock::now();

        /* Scope for lock_guard */
        {
            std::lock_guard<std::mutex> lock(pair_lock);
            pairs_copy = pairs;

            /* reset statistics */
            pairs.clear();
        }

        std::vector<PairStats> pair_stats = formatData(pairs_copy);
        std::vector<PairStats> pair_stats_ordered = orderData(pair_stats, order);

        // testPrintStats(pair_stats);
        display(pair_stats);
        /* Vector of sorted pairs */
        // std::vector<std::pair<std::string, PairData>> orderedPairs = orderPairs(pairsCopy, order);

        // display(orderedPairs);

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