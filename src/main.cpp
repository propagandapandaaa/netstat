#include "./include/options.h"
#include "./include/listener.h"

int main(int argc, char **argv)
{
    options options;
    parseArgs(argc, argv, &options);

    Mutex_Hashmap<std::string, struct PairData> pairs;
    // std::unordered_map<std::string, struct PairData> pairs;

    listener(options.interface.c_str(), pairs);

    printf("%s \n", options.interface.c_str());
    printf("%s \n", options.order.c_str());
}
