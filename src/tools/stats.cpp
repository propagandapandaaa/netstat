/*
Author
Name: Daniel Jacobs
Login: xjacob00
*/

#include "../include/stats.h"

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

    /* Converts unordered map to a vector that is usable in the display */
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
                            data.seconds = pair.second.seconds;
                            found = true;
                        }
                        /* Current tmp src_ip is the receiver */
                        if (tmp.src_ip == data.dst_ip && tmp.dst_ip == data.src_ip)
                        {
                            data.bytes_recv += pair.second.bytes;
                            data.packets_recv += pair.second.packets;

                            data.bytes_total += pair.second.bytes;
                            data.packets_total += pair.second.packets;
                            data.seconds = pair.second.seconds;
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
                tmp.packets_recv = 0;

                tmp.bytes_total = pair.second.bytes;
                tmp.packets_total = pair.second.packets;

                tmp.seconds = pair.second.seconds;

                result_data.emplace_back(tmp);
            }
        }
        return result_data;
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

    /* Merges new caputred packets with previously captured data */
    void addPairData(const std::unordered_map<std::string, PairData> &new_pairs,
                     std::unordered_map<std::string, PairData> &persistent_pairs,
                     const int timeout)
    {
        for (const auto &pair : new_pairs)
        {
            auto it = persistent_pairs.find(pair.first);
            if (it == persistent_pairs.end())
            {
                persistent_pairs[pair.first] = pair.second;
                persistent_pairs[pair.first].timeout = timeout;
                persistent_pairs[pair.first].updated = true;
                persistent_pairs[pair.first].seconds = 1;
            }
            else
            {
                it->second.bytes += pair.second.bytes;
                it->second.packets += pair.second.packets;
                it->second.timeout = timeout;
                it->second.updated = true;
                it->second.seconds++;
            }
        }
    }

    /* Clear timed out pairs */
    void processTimeouts(std::unordered_map<std::string, PairData> &persistent_pairs)
    {
        for (auto it = persistent_pairs.begin(); it != persistent_pairs.end();)
        {
            if (!it->second.updated)
            {
                it->second.timeout--;
                if (it->second.timeout <= 0)
                {
                    it = persistent_pairs.erase(it);
                    continue;
                }
            }
            it->second.updated = false;
            ++it;
        }
    }

}

/* Timer function, gets packets once per second and processes them */
void getStats(const char *orderString, std::unordered_map<std::string, PairData> &pairs, std::atomic<bool> &running)
{
    // Copy of pairs so that the mutex doesn't lock the original pairs for too long
    std::unordered_map<std::string, PairData> pairs_copy;

    // Persistent map of pairs, connection pairs get removed after n cycles (specified by DEFAULT_TIMEOUT)
    std::unordered_map<std::string, PairData> pairs_persistent;

    /* Wait for the first batch of packets */
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Amount of cycles of inactivity it takes to remove a connection from the display
    const int DEFAULT_TIMEOUT = 5;

    OrderBy order = setOrder(orderString);

    while (running)
    {
        const auto start = std::chrono::high_resolution_clock::now();

        /* Scope for lock_guard */
        {
            std::lock_guard<std::mutex> lock(pair_lock);
            pairs_copy = pairs;

            /* reset statistics */
            pairs.clear();
        }

        addPairData(pairs_copy, pairs_persistent, DEFAULT_TIMEOUT);

        processTimeouts(pairs_persistent);

        std::vector<PairStats> pair_stats = formatData(pairs_persistent);
        std::vector<PairStats> pair_stats_ordered = orderData(pair_stats, order);

        display(pair_stats);

        /* This section of the timer accounts for the processing time of the packets */
        const auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double, std::milli> elapsed = end - start;

        auto sleepDuration = std::chrono::milliseconds(1000) - elapsed;

        if (sleepDuration > std::chrono::milliseconds(0))
        {
            std::this_thread::sleep_for(sleepDuration);
        }
        else
        {
            /*  This is left empty because sometimes at larger packet volumes
                this condition can occur and throwing a runtime error stops the
                whole program. For now it is handled by ignoring this issue */

            // throw std::runtime_error("invalid timing");
        }
    }
}
