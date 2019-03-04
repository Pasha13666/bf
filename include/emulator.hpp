// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Created by pasha on 03.03.19.
//

#ifndef BF_EMULATOR_HPP
#define BF_EMULATOR_HPP

#include <memory>
#include "image.hpp"

class Emulator {
public:
    explicit Emulator(const BfppImage &image);

    void Run();

    bool ProtectedMode() { return protectedMode; }
    bool WordMode() { return wordMode; }
    bool Debug() { return debug; }
    bool Statistics() { return statistics; }

    void Statistics(bool mode) { statistics = mode; }
    void Debug(bool mode) { debug = mode; }
    void ProtectedMode(bool mode) { protectedMode = mode; }
    void WordMode(bool mode) { wordMode = mode; }

private:
    bool protectedMode = false, wordMode = false, debug = false, statistics = false;
    BfppImage image;
    std::array<uint16_t, 65536> memory {};

    uint16_t ip, ap;
};

#endif //BF_EMULATOR_HPP
