#include "./include/options.h"
#include "./include/listener.h"
#include "./include/stats.h"
#include <csignal>

std::atomic<bool> running(true);

void signalHandler(int signum)
{
    if (signum == SIGINT)
    {
        running = false;
    }
}

/* extern declared in locks.h */
std::mutex pair_lock;

int main(int argc, char **argv)
{

    struct sigaction sa;
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, nullptr) == -1)
    {
        perror("Failed to set signal handler");
        return 1;
    }

    options options;
    parseArgs(argc, argv, &options);

    initscr();

    std::unordered_map<std::string, PairData> pairs;

    /* Listens to incoming packets, adds them to hashmap */
    std::thread listener_thread([&options, &pairs]()
                                { listener(options.interface.c_str(), pairs, running); });

    /* Creates and displays statistics taken from the unordered hashmap */
    std::thread stats_thread([&options, &pairs]()
                             { getStats(options.order.c_str(), pairs, running); });

    listener_thread.join();
    stats_thread.join();

    /* CHECK WHAT CLEANUP NEEDS TO BE DONE :) */

    endwin();
    exit(0);
}
