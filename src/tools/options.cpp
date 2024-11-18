/*
Author
Name: Daniel Jacobs
Login: xjacob00
*/

#include "../include/options.h"

namespace
{
    /*  Check the validity of the parsed arguments
        NOTE: This does not check if the network interface is valid or not */
    void validateArgs(options *options)
    {
        if (strcmp(options->interface.c_str(), "default") == 0)
        {
            std::cout << "specify interface" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (strcmp(options->order.c_str(), "b") != 0 && strcmp(options->order.c_str(), "p") != 0)
        {
            std::cout << "invalid order, use b (bytes/s) or p (packets/s)" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    void printHelp()
    {
        std::cout << "\nManual: isa-top\n";
        std::cout << "============================\n\n";
        std::cout << "SYNTAX:\n";
        std::cout << "  ./isa-top -i <interface> [-s <sort_type>]\n\n";
        std::cout << "PARAMETERS:\n";
        std::cout << "  -i <interface>  : Required. Network interface to monitor (eth0, lo, ...)\n";
        std::cout << "  -s <sort_type>  : Optional. Sort output by:\n";
        std::cout << "                    b - total bytes (default)\n";
        std::cout << "                    p - total packets\n\n";
        std::cout << "EXAMPLE:\n";
        std::cout << "  ./isa-top -i eth0 -s p\n"
                  << std::endl;
    }

}

/*  Put command line arguments into a struct */
void parseArgs(int argc, char **argv, options *options)
{
    options->interface = "default";
    options->order = "b";
    int opt;
    while ((opt = getopt(argc, argv, "i:s:h")) != -1)
    {
        switch (opt)
        {
        case 'i':
            options->interface = optarg;
            break;
        case 's':
            options->order = optarg;
            break;
        case 'h':
            printHelp();
            exit(EXIT_SUCCESS);
        default:
            std::cout << "\nInvalid options\n"
                      << std::endl;
            printHelp();
            exit(EXIT_FAILURE);
        }
    }
    validateArgs(options);
}
