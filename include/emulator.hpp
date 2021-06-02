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
    explicit Emulator(const Image &image);

    void Run();

    bool ProtectedMode() { return protectedMode; }
    binary::Machine Machine() { return machine; }
    bool Debug() { return debug; }
    bool Statistics() { return statistics; }

    void Statistics(bool mode) { statistics = mode; }
    void Debug(bool mode) { debug = mode; }
    void ProtectedMode(bool mode) { protectedMode = mode; }
    void Machine(binary::Machine mode) { machine = mode; }

private:
    bool protectedMode = false, debug = false, statistics = false;
    Image image;
    std::array<uint16_t, 65536> memory {};
    binary::Machine machine;

    uint16_t ip, ap;

    void checkAp();
};

#endif //BF_EMULATOR_HPP
