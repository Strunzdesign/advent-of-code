#include <algorithm>
#include <array>
#include <boost/circular_buffer.hpp>
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

void Play(std::string_view a_FileName) {
    // Read the movements from the file
    std::cout << std::endl << "Reading movements from file \"" << a_FileName << "\"" << std::endl;
    std::ifstream l_InputStream { std::string { a_FileName } };
    std::string l_Line;

    std::int32_t l_HorPos = 0;
    std::int32_t l_DepthA = 0;
    std::int32_t l_DepthB = 0;
    std::int32_t l_AimB = 0;
    while (std::getline(l_InputStream, l_Line)) {
        if (l_Line.substr(0, 8) == "forward ") {
            const auto l_Value = atoi(l_Line.substr(8, (l_Line.size() - 8)).c_str());
            l_HorPos += l_Value;
            l_DepthB += (l_Value * l_AimB);
            assert(l_DepthB >= 0);
        } else if (l_Line.substr(0, 5) == "down ") {
            const auto l_Value = atoi(l_Line.substr(5, (l_Line.size() - 5)).c_str());
            l_DepthA += l_Value;
            l_AimB += l_Value;
            assert(l_DepthA >= 0);
        } else if (l_Line.substr(0, 3) == "up ") {
            const auto l_Value = atoi(l_Line.substr(3, (l_Line.size() - 3)).c_str());
            l_DepthA -= l_Value;
            l_AimB -= l_Value;
            assert(l_DepthA >= 0);
        } else {
            assert(false);
        } // else
    } // while
    
    std::cout << "Final position as product A = " << (l_HorPos * l_DepthA) << std::endl;
    std::cout << "Final position as product B = " << (l_HorPos * l_DepthB) << std::endl;
}

int main(int argc, char **argv) {
    Play("input-example.txt");
    Play("input.txt");
}
