#include <iostream>

int compiler_main(int argc, char **argv);
int emulator_main(int argc, char **argv);
int debugger_main(int argc, char **argv);
int flasher_main(int argc, char **argv);

int main(int argc, char **argv){
    while(argc) {
        std::string prog(argv[0]);

        if (prog == "bfpp")
            return compiler_main(argc, argv);
        if (prog == "bfrun")
            return emulator_main(argc, argv);
        if (prog == "bfdb")
            return debugger_main(argc, argv);
        if (prog == "bflash")
            return flasher_main(argc, argv);

        argc--;
        argv++;
    }

    std::cerr << "Please, specify tool bfpp, bfrun, bfdb or bflash as first argument" << std::endl;
    return -1;
}

