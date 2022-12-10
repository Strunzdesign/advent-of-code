#include <algorithm>
#include <array>
#include <cstddef>
#include <cassert>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <set>
#include <sstream>
#include <string_view>
#include <tuple>

class GPU {
public:
    void Pixel(std::int32_t a_SpritePos) {
        if (abs(a_SpritePos - m_X) < 2) {
            std::cout << "#";
        } else {
            std::cout << ".";
        } // else
        
        // Update
        ++m_X;
        if (m_X == 40) {
            m_X = 0;
            std::cout << std::endl;
        } // if
    }
    
private:
    std::uint32_t m_X = 0;
};

class CPU {
public:
    CPU(const std::function<void(std::uint32_t a_Cycle, std::int32_t a_RegisterX)>& a_DebugHook) {
        assert(a_DebugHook);
        m_DebugHook = a_DebugHook;
    }
    
    void Process(std::string a_Instruction) {
        if (a_Instruction == "noop") {
            Cycle();
        } else if (a_Instruction.substr(0, 5) == "addx ") {
            Cycle();
            Cycle();
            m_RegisterX += std::atoi(a_Instruction.substr(5, (a_Instruction.size() - 5)).c_str());
        } else {
            assert(false);
        } // else
    }
    
private:
    void Cycle() {
        // Draw -> riddle B, then update CPU
        m_GPU.Pixel(m_RegisterX); // X is position of sprite
        ++m_CyclesPassed;
        m_DebugHook(m_CyclesPassed, m_RegisterX);
    }
    
    GPU m_GPU;
    std::function<void(std::uint32_t a_Cycle, std::int32_t a_RegisterX)> m_DebugHook;
    std::int32_t m_RegisterX = 1;
    std::uint32_t m_CyclesPassed = 0;
};

void Play(std::string_view a_FileName) {
    // Read the CPU instructions
    std::cout << std::endl << "Feeding CPU instructions from file \"" << a_FileName << "\" to CPU" << std::endl;
    std::ifstream l_InputStream { std::string { a_FileName } };
    std::string l_Line;

    std::int32_t l_SignalStrength = 0;
    auto l_CPU = CPU { [&l_SignalStrength](std::uint32_t a_Cycle, std::int32_t a_RegisterX) {
        if ((a_Cycle == 20) || (a_Cycle == 60) || (a_Cycle == 100) || (a_Cycle == 140) || (a_Cycle == 180) || (a_Cycle == 220)) {
            l_SignalStrength += (a_Cycle * a_RegisterX);
        } // if
    } };
    
    while (std::getline(l_InputStream, l_Line)) {
        // Parse one movement, but then only move the head of rope one step at a time
        l_CPU.Process(l_Line);  
    } // while
    
    std::cout << "Signal strength = " << l_SignalStrength << std::endl;
}

int main(int argc, char **argv) {
    Play("input-example.txt");
    Play("input.txt");
}
