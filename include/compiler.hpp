// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Created by pasha on 03.03.19.
//

#ifndef BF_COMPILER_HPP
#define BF_COMPILER_HPP

#include <istream>
#include "image.hpp"

const char *const FORMAT_NAMES[4][4] = {
        "bf", "bfk", "brainfuck", "source",
        "bfa", "asm", "s", "assembly",
        "hex", "c", "cpp", "hex",
        "bfi", "img", "bin", "binary"
};

class Compiler {
public:
    enum class Format {
        SOURCE, // .bf, .bfk, .bfe, .bfpp
        ASSEMBLY, // .bfa
        HEX, // .hex, .txt
        IMAGE // .bfi, a.out
    };

    Compiler() = default;
    void Compile(std::fstream &in, Format inf, std::fstream &out, Format of);

    bool OptimizeClearing(){ return optimizeClearing; }
    bool OptimizeJoining(){ return optimizeJoining; }

    void OptimizeClearing(bool v) { optimizeClearing = v; }
    void OptimizeJoining(bool v) { optimizeJoining = v; }

private:
    void WriteSource(std::vector<Command>& output, std::fstream &out);
    void WriteHex(std::vector<Command>& output, std::fstream &out);
    void WriteAssembly(std::vector<Command>& output, std::fstream &out);
    void ReadAssembly(std::vector<Command>& output, std::fstream &in);

    void Translate(std::istream &in, std::vector<Command>& output);
    void Linking(std::vector<Command>& output);
    void OptimizeClear(std::vector<Command> &output);

    void FindLoopEnd(std::vector<Command> &Output, size_t CurrentIp);

private:
    bool optimizeClearing = false, optimizeJoining = false;

};

#endif //BF_COMPILER_HPP
