#include "./include/options.h"
#include "./include/listener.h"
#include "./include/stats.h"

/* extern declared in locks.h */
std::mutex pair_lock;

int main(int argc, char **argv)
{
    options options;
    parseArgs(argc, argv, &options);

    std::unordered_map<std::string, struct PairData> pairs;
    // std::unordered_map<std::string, struct PairData> pairs;

        std::thread listener_thread([&options, &pairs]()
                                { listener(options.interface.c_str(), pairs); });

    std::thread stats_thread([&options, &pairs]()
                             { getStats(options.order.c_str(), pairs); });

    listener_thread.join();
    stats_thread.join();

    printf("%s \n", options.interface.c_str());
    printf("%s \n", options.order.c_str());
}
