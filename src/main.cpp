#include "./include/options.h"
#include "./include/listener.h"
#include "./include/stats.h"
#include <csignal>

std::atomic<bool> running(true);

void cleanup() noexcept
{
    std::cout << "Cleaning up\n";
    endwin();
}

void signalHandler(int signum) noexcept
{
    if (signum == SIGINT)
    {
        running = false;
    }
}

std::mutex pair_lock;

int main(int argc, char **argv)
{

    struct sigaction sa;
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, nullptr) == -1)
    {
        throw std::runtime_error("Failed to set signal handler");
    }

    options options;
    parseArgs(argc, argv, &options);

    std::unordered_map<std::string, PairData> pairs;
    std::exception_ptr thread_exception;
    initscr();

    /* Listens to incoming packets, adds them to hashmap */
    std::thread listener_thread([&options, &pairs, &thread_exception]()
                                {
            try {
                listener(options.interface.c_str(), pairs, running);
            } catch (const std::runtime_error &e) {
                std::cout << "Listener exception caught\n";
                cleanup();
                fprintf(stderr, "Error: %s\n", e.what());
                thread_exception = std::current_exception();
                running = false;
            } });

    /* Creates and displays statistics taken from the unordered hashmap */
    std::thread stats_thread([&options, &pairs, &thread_exception]()
                             {
            try {
                getStats(options.order.c_str(), pairs, running);
            } catch (...) {
                thread_exception = std::current_exception();
                running = false;
            } });

    listener_thread.join();
    stats_thread.join();

    return 0;
}
