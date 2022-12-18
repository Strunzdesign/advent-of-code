#include <algorithm>
#include <array>
#include <boost/tokenizer.hpp>
#include <cstddef>
#include <cassert>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <regex>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <tuple>

using Coordinate = std::tuple<std::int32_t, std::int32_t, std::int32_t>;

constexpr static auto s_Directions = std::array<Coordinate, 6> {
    Coordinate { 1, 0, 0 },
    Coordinate { -1, 0, 0 },
    Coordinate { 0, 1, 0 },
    Coordinate { 0, -1, 0 },
    Coordinate { 0, 0, 1 },
    Coordinate { 0, 0, -1 }
};

auto operator+(const Coordinate& a_Left, const Coordinate& a_Right) {
    return Coordinate { (std::get<0>(a_Left) + std::get<0>(a_Right)), (std::get<1>(a_Left) + std::get<1>(a_Right)), (std::get<2>(a_Left) + std::get<2>(a_Right)) };
}

bool InRange(const Coordinate& a_Input) {
    return (
        (std::get<0>(a_Input) >= -1) && (std::get<0>(a_Input) <= 21) &&
        (std::get<1>(a_Input) >= -1) && (std::get<1>(a_Input) <= 21) &&
        (std::get<2>(a_Input) >= -1) && (std::get<2>(a_Input) <= 21));
}

void Play(std::string_view a_FileName) {
    // Read lava droplet information
    std::cout << std::endl << "Reading lava droplet information from file \"" << a_FileName << "\"" << std::endl;
    std::ifstream l_InputStream { std::string { a_FileName } };
    std::string l_Line;

    std::set<Coordinate> l_Droplets;
    while (std::getline(l_InputStream, l_Line)) {
        auto l_Input = std::stringstream { l_Line };
        std::string l_X;
        auto l_ParseResultX = static_cast<bool>(std::getline(l_Input, l_X, ','));
        assert(l_ParseResultX);
        std::string l_Y;
        auto l_ParseResultY = static_cast<bool>(std::getline(l_Input, l_Y, ','));
        assert(l_ParseResultY);
        std::string l_Z;
        auto l_ParseResultZ = static_cast<bool>(std::getline(l_Input, l_Z, ','));
        assert(l_ParseResultZ);
        l_Droplets.insert(Coordinate { std::atoi(l_X.c_str()), std::atoi(l_Y.c_str()), std::atoi(l_Z.c_str()) });
    } // while
    
    // Perform flood
    auto l_Water = std::set<Coordinate>{};
    auto l_TestCoordinates = std::set<Coordinate>{};
    l_TestCoordinates.insert(Coordinate { 0,  0,  0 } );
    while (!l_TestCoordinates.empty()) {
        auto l_NextTestCoordinates = std::set<Coordinate>{};
        for (const auto& l_TestCoordinate: l_TestCoordinates) {
            // Ignore any out-of-bounds coordinates as well as any known water and lava cubes
            if (InRange(l_TestCoordinate) && (!l_Water.count(l_TestCoordinate) && (!l_Droplets.count(l_TestCoordinate)))) {
                // Swoosh
                l_Water.insert(l_TestCoordinate);
                for (const auto& l_Direction: s_Directions) {
                    l_NextTestCoordinates.insert(l_TestCoordinate + l_Direction);
                } // for
            } // if
        } // for
        
        l_TestCoordinates = l_NextTestCoordinates;
    } // while
    
    // Test each lava droplet
    auto l_ExposedVerticesA = 0;
    auto l_ExposedVerticesB = 0;
    for (const auto& l_Droplet: l_Droplets) {
        for (const auto& l_Direction: s_Directions) {
            if (!l_Droplets.count(l_Droplet + l_Direction)) {
                ++l_ExposedVerticesA; // neighbor is no droplet, must be "air" or "water": count for riddle A
            } // if
            
            if (l_Water.count(l_Droplet + l_Direction)) {
                ++l_ExposedVerticesB; // neighbor is water: count for riddle B
            } // if
        } // for
    } // for

    std::cout << "A: exposed vertices = " << l_ExposedVerticesA << std::endl;
    std::cout << "B: exposed vertices = " << l_ExposedVerticesB << std::endl; // NOT 2057 too low
}

int main(int argc, char **argv) {
    Play("input-example.txt");
    Play("input.txt");
}
