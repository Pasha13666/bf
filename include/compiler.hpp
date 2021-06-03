// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Created by pasha on 03.03.19.
//

#ifndef BF_COMPILER_HPP
#define BF_COMPILER_HPP

#include <fstream>
#include "image.hpp"

class Compiler {
public:

    Compiler() = default;

    /*       Out SOURCE ASSEMBLY HEX IMAGE
     *    In    *-------------------------
     *   SOURCE |  V       N      V    V
     * ASSEMBLY |  V       N      V    V
     *      HEX |  X       X      X    X
     *    IMAGE |  X       X      X    V
     *
     *    V - Supported
     *    X - Not supported
     *    N - Not implemented
     */
    void Compile(std::fstream &in, binary::Format inf, std::fstream &out, binary::Format of);

    [[nodiscard]] bool OptimizeClearing() const { return optimizeClearing; }
    [[nodiscard]] bool OptimizeJoining() const { return optimizeJoining; }

    void OptimizeClearing(bool v) { optimizeClearing = v; }
    void OptimizeJoining(bool v) { optimizeJoining = v; }

private:
    static void WriteSource(std::vector<Command>& output, std::fstream &out);
    static void WriteHex(std::vector<Command>& output, std::fstream &out);
    void WriteAssembly(std::vector<Command>& output, std::fstream &out);

    void Translate(std::istream &in, std::vector<Command>& output) const;
    static void Linking(std::vector<Command>& output);
    static void OptimizeClear(std::vector<Command> &output);

    static void FindLoopEnd(std::vector<Command> &Output, size_t CurrentIp);

private:
    bool optimizeClearing = false, optimizeJoining = false;

};

#endif //BF_COMPILER_HPP
