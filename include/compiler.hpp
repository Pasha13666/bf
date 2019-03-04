// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Created by pasha on 03.03.19.
//

#ifndef BF_COMPILER_HPP
#define BF_COMPILER_HPP

#include <istream>
#include "image.hpp"

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

    bool ExtendedCommands(){ return extendedCommands; }
    bool OptimizeClearing(){ return optimizeClearing; }
    bool OptimizeJoining(){ return optimizeJoining; }
    bool OptimizeJoiningCtrlio(){ return optimizeJoiningCtrlio; }

    void ExtendedCommands(bool v) { extendedCommands = v; }
    void OptimizeClearing(bool v) { optimizeClearing = v; }
    void OptimizeJoining(bool v) { optimizeJoining = v; }
    void OptimizeJoiningCtrlio(bool v) { optimizeJoiningCtrlio = v; }

private:
    void WriteSource(std::vector<Command>& output, std::fstream &out);
    void WriteHex(std::vector<Command>& output, std::fstream &out);
    void WriteAssembly(std::vector<Command>& output, std::fstream &out);

    void Translate(std::istream &in, std::vector<Command>& output);
    void Linking(std::vector<Command>& output);
    void OptimizeClear(std::vector<Command> &output);

    bool IsSymbol(char ch);
    void FindLoopEnd(std::vector<Command> &Output, size_t CurrentIp);

private:
    bool extendedCommands = false, optimizeClearing = false, optimizeJoining = false, optimizeJoiningCtrlio = false;

};

#endif //BF_COMPILER_HPP
