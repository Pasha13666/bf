// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Created by pasha on 07.03.19.
//


#include "emulator.hpp"
#include "cxxopts.hpp"
#include <fstream>


int emulator_main(int argc, char **argv) {
    cxxopts::Options options("bfrun", "Brainfuck++ emulator");
    options.add_options()
            ("i,input", "Input file", cxxopts::value<std::string>())
            ("w,8bit", "Force 8 bit mode")
            ("W,16bit", "Force 16 bit mode")
            ("p,protected", "Run in protected mode")
            ("s,statistics", "Count executed instructions")
            ("v,verbose", "Produce more output")
            ("d,debug", "Enable debugging")
            ("D,debugger-socket", "Socket for debugging", cxxopts::value<std::string>()->default_value("/tmp/bfdebugger.sock"))
            ("h,help", "Show this message");

    options.show_positional_help();
    try {
        auto result = options.parse(argc, argv);

        if (result["help"].as<bool>()) {
            std::cout << options.help();
            return 0;
        }

        std::fstream input(result["input"].as<std::string>(), std::ios::in | std::ios::binary);
        if (!input.good()) {
            std::cerr << "Cannot read input file" << std::endl;
            return 1;
        }

        Image img(input);
        Emulator e(img);

        bool m8 = result["8bit"].as<bool>();
        bool m16 = result["16bit"].as<bool>();
        if (m8 && m16) {
            std::cerr << "Options --8bit and --16bit present together" << std::endl;
            return -1;
        }

        if (m8)
            e.Machine(binary::MACHINE_8BIT);
        else if (m16)
            e.Machine(binary::MACHINE_16BIT);

        if (result["protected"].as<bool>())
            e.ProtectedMode(true);

        if (result["statistics"].as<bool>())
            e.Statistics(true);

        if (result["verbose"].as<bool>())
            e.Verbose(true);

        if (result["debug"].as<bool>())
            e.Debug(result["debugger-socket"].as<std::string>());

        e.Run();

    } catch (cxxopts::OptionException &e) {
        std::cerr << e.what() << std::endl;
        std::cerr << options.help();
        return -1;
    } catch (std::domain_error &e) {
        std::cerr << "Option ‘input’ not present" << std::endl;
        std::cerr << options.help();
        return -1;
    }

    return 0;
}
