// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Created by pasha on 07.03.19.
//
#include "compiler.hpp"
#include <string>
#include <iostream>
#include "cxxopts.hpp"
#include <filesystem>


binary::Format detect_format(const std::string &s) {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            if (s == binary::FORMAT_NAMES[i][j])
                return binary::Format(i);

    throw std::runtime_error("Unknown file format '" + s + "'");
}

int compiler_main(int argc, char **argv) {
    cxxopts::Options options("bfpp", "Brainfuck++ compiler");
    options.add_options()
            ("O,optimize", "Optimization level (default is 1)", cxxopts::value<unsigned int>()->default_value("1"))
            ("o,output", "Output file", cxxopts::value<std::filesystem::path>()->default_value("a.bin"))
            ("i,input", "Input file", cxxopts::value<std::filesystem::path>())
            ("X,output-format", "Format of output file", cxxopts::value<std::string>())
            ("I,input-format", "Format of input file", cxxopts::value<std::string>())
            ("v,verbose", "Produce more output")
            ("h,help", "Show this message");

    options.show_positional_help();
    try {
        auto result = options.parse(argc, argv);
        Compiler c {};

        if (result["help"].as<bool>()) {
            std::cout << options.help();
            return 0;
        }

        std::fstream input(result["input"].as<std::filesystem::path>(), std::ios::in | std::ios::binary);
        if (!input.good()) {
            std::cerr << "Cannot read input file" << std::endl;
            return 1;
        }

        std::fstream output(result["output"].as<std::filesystem::path>(), std::ios::out | std::ios::binary);
        if (!output.good()) {
            std::cerr << "Cannot write output file" << std::endl;
            return 1;
        }

        switch (result["optimize"].as<unsigned int>()) {
            default:
            case 2:
                c.OptimizeClearing(true);
            case 1:
                c.OptimizeJoining(true);
            case 0:
                break;
        }

        binary::Format inf, of;

        if (result.count("input-format") != 0)
            inf = detect_format(result["input-format"].as<std::string>());
        else inf = detect_format(result["input"].as<std::filesystem::path>().extension().string().substr(1));

        if (result.count("output-format") != 0)
            of = detect_format(result["output-format"].as<std::string>());
        else of = detect_format(result["output"].as<std::filesystem::path>().extension().string().substr(1));

        // verbose

        c.Compile(input, inf, output, of);

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
