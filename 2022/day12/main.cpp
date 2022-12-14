#include <algorithm>
#include <array>
#include <boost/tokenizer.hpp>
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

class Terrain {
public:
    // Types
    using Coordinate = std::pair<std::uint32_t, std::uint32_t>; // Y, X
    using HeightEntry = std::pair<std::uint8_t, std::optional<std::uint32_t>>; // height, steps (if visited)
    using CoordinateToTest = std::tuple<std::uint32_t, std::uint32_t, std::uint8_t>; // Y, X, height from originating neighbor
    
    void AddLine(std::string_view a_Line) {
        const auto l_Y = m_HeightMap.size();
        auto& l_DataLine = m_HeightMap.emplace_back(std::vector<HeightEntry>{} );
        for (auto l_Char: a_Line) {
            if (l_Char == 'S') {
                // Start field, has height 0
                m_StartCoords = Coordinate { l_Y, l_DataLine.size() };
                l_DataLine.emplace_back(HeightEntry { 0, std::nullopt } );
            } else if (l_Char == 'E') {
                // Final field, has height 26
                m_FinalCoords = Coordinate { l_Y, l_DataLine.size() };
                l_DataLine.emplace_back(HeightEntry { 26, std::nullopt } );
            } else {
                l_DataLine.emplace_back(HeightEntry { (l_Char - 'a'), std::nullopt } );
            } // else
        } // for
    }
    
    void TracePath() {
        // Go from the final back to the start
        assert(m_FinalCoords.has_value());
        const auto& [l_Y, l_X] = m_FinalCoords.value();
        std::vector<CoordinateToTest> l_CoordinatesToTest { CoordinateToTest { l_Y, l_X, 26 } };
        Trace(l_CoordinatesToTest, 0); // Recursive, start with step 0
    }
    
    [[nodiscard]] auto GetStepsAtDestinationA() const noexcept {
        assert(m_StartCoords.has_value());
        const auto& [l_Y, l_X] = m_StartCoords.value();
        const auto& [l_Height, l_oStepNbr] = m_HeightMap[l_Y][l_X];
        assert(l_oStepNbr.has_value());
        return l_oStepNbr.value();
    }
    
    [[nodiscard]] auto GetStepsB() const noexcept {
        return m_StepsB.value();
    }
    
private:
    void Trace(const std::vector<CoordinateToTest>& a_CoordinatesToTest, std::uint32_t a_StepNbr) {
        std::vector<CoordinateToTest> l_NextCoordinates;
        for (const auto& l_CoordinateToTest: a_CoordinatesToTest) {
            // Test this coordinate
            const auto& [l_TestY, l_TestX, l_FromHeight] = l_CoordinateToTest;
            try {
                auto& [l_Height, l_oStepNbr] = m_HeightMap.at(l_TestY).at(l_TestX);
                if (!l_oStepNbr.has_value()) {
                    // New unvisited field
                    if ((l_Height > l_FromHeight) || ((l_FromHeight - l_Height) <= 1)) {
                        l_oStepNbr = a_StepNbr;
                        if ((l_Height == 0) && !m_StepsB.has_value()) {
                            m_StepsB = a_StepNbr;
                        } // if
                        
                        l_NextCoordinates.push_back(CoordinateToTest { l_TestY - 1, l_TestX,     l_Height } );
                        l_NextCoordinates.push_back(CoordinateToTest { l_TestY + 1, l_TestX,     l_Height } );
                        l_NextCoordinates.push_back(CoordinateToTest { l_TestY,     l_TestX - 1, l_Height } );
                        l_NextCoordinates.push_back(CoordinateToTest { l_TestY,     l_TestX + 1, l_Height } );
                    } // if
                } // if
            } catch (...) {
                // Discards any out of bounds coordinate
            } // catch
        } // while
        
        if (!l_NextCoordinates.empty()) {
            Trace(l_NextCoordinates, ++a_StepNbr);
        } // if
    }
    
    //
    std::optional<Coordinate> m_StartCoords;
    std::optional<Coordinate> m_FinalCoords;
    std::vector<std::vector<HeightEntry>> m_HeightMap;
    std::optional<std::uint32_t> m_StepsB;
};

void Play(std::string_view a_FileName) {
    // Read the height map
    std::cout << std::endl << "Reading height map from file \"" << a_FileName << "\"" << std::endl;
    std::ifstream l_InputStream { std::string { a_FileName } };
    std::string l_Line;

    Terrain l_Terrain;
    while (std::getline(l_InputStream, l_Line)) {
        l_Terrain.AddLine(l_Line);
    } // while
    
    l_Terrain.TracePath();
    std::cout << "A: steps at destination = " << l_Terrain.GetStepsAtDestinationA() << std::endl;
    std::cout << "B: first at ground level after " << l_Terrain.GetStepsB() << " steps" << std::endl;
}

int main(int argc, char **argv) {
    Play("input-example.txt");
    Play("input.txt");
}
