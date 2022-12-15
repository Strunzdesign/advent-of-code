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
#include <stdexcept>
#include <string_view>
#include <tuple>

using Coordinate = std::pair<std::uint32_t, std::uint32_t>;

class Cave {
public:
    void AddRockFormation(std::string_view a_Line) {
        // Split strings of type "503,4 -> 502,4 -> 502,9 -> 494,9"
        boost::char_separator<char> l_Separator { " -> " };
        const auto l_Tokenizer = boost::tokenizer<boost::char_separator<char>> { a_Line, l_Separator };
        auto l_oLastCoordinate = std::optional<Coordinate>{};
        for (const auto &l_Token: l_Tokenizer) {
            auto l_Input = std::stringstream { l_Token };
            std::string l_X;
            const auto l_ParseResultX = static_cast<bool>(std::getline(l_Input, l_X, ','));
            assert(l_ParseResultX);
            std::string l_Y;
            const auto l_ParseResultY = static_cast<bool>(std::getline(l_Input, l_Y, ','));
            assert(l_ParseResultY);
            const auto l_ThisCoordinate = Coordinate { std::atoi(l_X.c_str()), std::atoi(l_Y.c_str()) };
            AddLineOfRock(l_oLastCoordinate, l_ThisCoordinate);
            l_oLastCoordinate = l_ThisCoordinate;
        } // for
    }
    
    void AddLineOfRock(const std::optional<Coordinate>& a_oFromCoordinate, const Coordinate& a_ToCoordinate) {
        if (!a_oFromCoordinate.has_value()) {
            return;
        } //
        
        const auto& [l_FromX, l_FromY] = a_oFromCoordinate.value();
        const auto& [l_DestX, l_DestY] = a_ToCoordinate;
        if (l_FromY == l_DestY) {
            // Plot horizontally
            const auto l_MinX = std::min(l_FromX, l_DestX);
            const auto l_MaxX = std::max(l_FromX, l_DestX);
            for (auto l_X = l_MinX; l_X <= l_MaxX; ++l_X) {
                m_CaveLayout[l_FromY][l_X] = Voxel::Rock;
                m_MaxY = std::max(m_MaxY, l_FromY);
            } // for
        } else if (l_FromX == l_DestX) {
            // Plot vertically
            const auto l_MinY = std::min(l_FromY, l_DestY);
            const auto l_MaxY = std::max(l_FromY, l_DestY);
            for (auto l_Y = l_MinY; l_Y <= l_MaxY; ++l_Y) {
                m_CaveLayout[l_Y][l_FromX] = Voxel::Rock;
                m_MaxY = std::max(m_MaxY, l_Y);
            } // for
        } else {
            assert(false);
        } // else
    }

    [[nodiscard]] auto HasSpace(const Coordinate& a_Position) const noexcept {
        const auto& [l_X, l_Y] = a_Position;
        const auto l_LineIt = m_CaveLayout.find(l_Y);
        if (l_LineIt != m_CaveLayout.end()) {
            const auto l_VoxelIt = l_LineIt->second.find(l_X);
            if (l_VoxelIt != l_LineIt->second.end()) {
                return (l_VoxelIt->second == Voxel::Air);
            } // if
        } // if
        
        return true;
    }
    
    void Settle(const Coordinate& a_Position) noexcept {
        const auto& [l_X, l_Y] = a_Position;
        m_CaveLayout[l_Y][l_X] = Voxel::Sand;
    }
    
    [[nodiscard]] auto GetMaxY() const noexcept { return m_MaxY; }
    
private:
    // Types
    enum class Voxel {
        Air = 0,
        Rock,
        Sand
    };
    
    std::map<std::uint32_t, std::map<std::uint32_t, Voxel>> m_CaveLayout; // [Y,X] -> Voxel
    std::uint32_t m_MaxY = 0;
};

void TraceOneSand(Cave& a_Cave) {
    // Trace
    const auto l_Flow = [&a_Cave](const Coordinate& a_Position) {
        const auto& [l_X, l_Y] = a_Position;
        if ((l_Y > 1000) || (!a_Cave.HasSpace(a_Position))) {
            throw std::out_of_range("");
        } // if
        
        if (const auto l_Test = Coordinate { l_X, (l_Y + 1) }; a_Cave.HasSpace(l_Test)) {
            // One step down
            return l_Test;
        } else if (const auto l_Test = Coordinate { (l_X - 1), (l_Y + 1) }; a_Cave.HasSpace(l_Test)) {
            // One step diagonally down left
            return l_Test;
        } else if (const auto l_Test = Coordinate { (l_X + 1), (l_Y + 1) }; a_Cave.HasSpace(l_Test)) {
            // One step diagonally down right
            return l_Test;
        } else {
            // Settle
            return a_Position;
        } // else
    };
    
    auto l_CurPos = Coordinate { 500, 0 };
    while (true) {
        const auto l_NextPos = l_Flow(l_CurPos);
        if (l_CurPos == l_NextPos) {
            // Settled
            a_Cave.Settle(l_CurPos);
            break;
        } // if
        
        l_CurPos = l_NextPos;
    } // while
}

[[nodiscard]] auto Flow(Cave& a_Cave, std::uint32_t a_StartAtStep) noexcept {
    try {
        while (true) {
            TraceOneSand(a_Cave);
            ++a_StartAtStep;
        } // while
    } catch (const std::out_of_range&) {
        //
    } // catch
    
    return a_StartAtStep;
}

void Play(std::string_view a_FileName) {
    // Read the cave layout
    std::cout << std::endl << "Reading rock formations from file \"" << a_FileName << "\"" << std::endl;
    std::ifstream l_InputStream { std::string { a_FileName } };
    std::string l_Line;

    auto l_Cave = Cave{};
    while (std::getline(l_InputStream, l_Line)) {
        l_Cave.AddRockFormation(l_Line);
    } // while
    
    // Pour in sand...
    auto l_Steps = Flow(l_Cave, 0);
    std::cout << "A: Amount of sand = " << l_Steps << std::endl;
    
    // Ok, now add the bottom line and continue to pour in sand
    l_Cave.AddLineOfRock(Coordinate { (498 - l_Cave.GetMaxY()), (l_Cave.GetMaxY() + 2) }, Coordinate { (502 + l_Cave.GetMaxY()), (l_Cave.GetMaxY() + 2) });
    l_Steps = Flow(l_Cave, l_Steps);
    std::cout << "B: Amount of sand = " << l_Steps << std::endl;
}

int main(int argc, char **argv) {
    Play("input-example.txt");
    Play("input.txt");
}
