#include "../include/options.h"

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

/*  Put command line arguments into a struct */
void parseArgs(int argc, char **argv, options *options)
{
    options->interface = "default";
    options->order = "b";
    int opt;
    while ((opt = getopt(argc, argv, "i:s:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            options->interface = optarg;
            break;
        case 's':
            options->order = optarg;
            break;
        default:
            std::cout << "invalid options" << std::endl;
            exit(EXIT_FAILURE);
            // Print help or smth like that
        }
    }
    validateArgs(options);
}