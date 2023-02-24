#include <irap/bin/parse.hpp>

#include <iostream>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <array>

#include <getopt.h>

int help() {
    std::cout << "Usage: irap-cat [OPTION]... [FILE]...\n"
        "Concatinate the headers to standard output.\n"
        "\n"
        "-t, --type       Type of irap [ascii|binary]\n"
        "    --help       display this help and exit\n"
        "\n"
    ;

    return 0;
}

void print_bin( void* buffer, std::size_t size )
{
    irap::bin::Header header;
    irap::bin::read_header(buffer, size, header);

    std::cout << std::fixed <<
        "idflag : " << header.idflag << "\n" <<
        "ny     : " << header.ny     << "\n" <<
        "xori   : " << header.xori   << "\n" <<
        "xmax   : " << header.xmax   << "\n" <<
        "xinc   : " << header.xinc   << "\n" <<
        "yinc   : " << header.yinc   << "\n" <<
        "nx     : " << header.nx     << "\n" <<
        "rot    : " << header.rot    << "\n" <<
        "x0ori  : " << header.x0ori  << "\n" <<
        "y0ori  : " << header.y0ori  << "\n" <<
    std::endl;
}

void print_ascii( void* buffer, std::size_t size )
{
    std::cout << "irap ascii not implemented\n" << std::endl;
}

int main( int argc, char** argv )
{
    std::string filetype;
    struct option long_options[] = {
        { "type",    required_argument, 0,        't' },
        { "help",    no_argument,       0,        'h' },
        { 0, 0, 0, 0 }
    };

    while( true ) {
        int option_index = 0;
        int c = getopt_long(argc, argv, "t:", long_options, &option_index);

        if( c == -1 ) break;

        switch( c ) {
            case  0:  break;
            case 'h': exit( help() );
            case 't': {
                filetype = std::string(optarg);
                break;
            }
            default:
                exit( help() );
        }
    }

    if (filetype.size() == 0) {
        std::cout << "Missing required option -t --type\n\n";
        help();
        return EXIT_FAILURE;
    }

    if (argc - optind < 1) {
        std::cout <<
            "missing file operand\n" <<
            "Try 'irap-cat --help' for more information." <<
        std::endl;
        return EXIT_FAILURE;
    }

    std::array< unsigned int, irap::bin::HEADER_SIZE> buffer;
    for (int i = optind; i < argc; ++i) {
        FILE* fp = std::fopen( argv[i], "r" );
        std::size_t read = std::fread(buffer.data(), 1, buffer.size(), fp);
        std::fclose(fp);

        if (read != irap::bin::HEADER_SIZE) {
            std::cout << "Could not read enough bytes from file" << std::endl;
            return EXIT_FAILURE;
        }

        if (filetype.compare("binary") == 0) {
            try {
                print_bin(buffer.data(), buffer.size());
            } catch (std::exception const& e) {
                std::cout << e.what() << std::endl;
                return EXIT_FAILURE;
            }
        } else if (filetype.compare("ascii") == 0) {
            try {
                print_ascii(buffer.data(), buffer.size());
            } catch (std::exception const& e) {
                std::cout << e.what() << std::endl;
                return EXIT_FAILURE;
            }
        } else {
            std::cout << "Unexpected filetype" << std::endl;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
