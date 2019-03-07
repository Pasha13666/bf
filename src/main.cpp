#include <iostream>
#include <cxxopts.hpp>
#include <fstream>
#include <compiler.hpp>
#include <emulator.hpp>
#include <filesystem>

Compiler::Format detect_format(const std::string &s) {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            if (s == FORMAT_NAMES[i][j])
                return Compiler::Format(i);

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

        std::fstream input(result["input"].as<std::filesystem::path>(), std::ios::in);
        if (!input.good()) {
            std::cerr << "Cannot read input file" << std::endl;
            return 1;
        }

        std::fstream output(result["output"].as<std::filesystem::path>(), std::ios::out);
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

        Compiler::Format inf, of;

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

int emulator_main(int argc, char **argv) {
    cxxopts::Options options("bfrun", "Brainfuck++ emulator");
    options.add_options()
            ("i,input", "Input file", cxxopts::value<std::string>())
            ("w,8bit", "Force 8 bit mode")
            ("W,16bit", "Force 16 bit mode")
            ("p,protected", "Run in protected mode")
            ("s,statistics", "Count executed instructions")
            ("v,verbose", "Produce more output")
            ("h,help", "Show this message");

    options.show_positional_help();
    try {
        auto result = options.parse(argc, argv);

        if (result["help"].as<bool>()) {
            std::cout << options.help();
            return 0;
        }

        std::fstream input(result["input"].as<std::string>(), std::ios::in);
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
            e.Debug(true);

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

int main(int argc, char **argv){
    std::string prog(argv[0]);

    if (prog == "bfpp")
        return compiler_main(argc, argv);
    if (prog == "bfrun")
        return emulator_main(argc, argv);

    argc--;
    argv++;
    if (argc == 0){
        std::cerr << prog << ": Please, specify tool bfpp or bfrun as first argument" << std::endl;
        return -1;
    }

    return main(argc, argv);
}

