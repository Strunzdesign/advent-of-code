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

using Coordinate = std::pair<std::int32_t, std::int32_t>;

[[nodiscard]] std::uint64_t CalcMHDistance(const Coordinate& a_A, const Coordinate& a_B) noexcept {
    return (std::abs(a_A.first - a_B.first) + std::abs(a_A.second - a_B.second));
}

class Sensor {
public:
    Sensor(
        const Coordinate& a_Self,
        const Coordinate& a_Beacon
    ) : m_Self { a_Self },
        m_Beacon { a_Beacon },
        m_MHDistance { CalcMHDistance(a_Self, a_Beacon) } {}
    
    [[nodiscard]] auto HasBeaconAt(const Coordinate& a_TestCoord) const noexcept {
        return (a_TestCoord == m_Beacon);
    }
    
    [[nodiscard]] auto CoverageTest(const Coordinate& a_TestCoord) const noexcept {
        return (CalcMHDistance(m_Self, a_TestCoord) <= m_MHDistance);
    }
    
    [[nodiscard]] auto StepAroundPerimeter(const std::optional<Coordinate>& a_oLastPos) const noexcept -> std::optional<Coordinate> {
        const auto& [l_SelfX, l_SelfY] = m_Self;
        if (!a_oLastPos) {
            // First step around the perimeter, start on top
            return Coordinate { l_SelfX, (l_SelfY - m_MHDistance - 1) };
        } // if
        
        const auto& [l_TestX, l_TestY] = a_oLastPos.value();
        if ((l_TestY < l_SelfY) && (l_TestX >= l_SelfX)) {
            return Coordinate { l_TestX + 1, l_TestY + 1 }; // upper right, downwards
        } else if ((l_TestY >= l_SelfY) && (l_TestX > l_SelfX)) {
            return Coordinate { l_TestX - 1, l_TestY + 1 };  // lower right, downwards
        } else if ((l_TestY > l_SelfY) && (l_TestX <= l_SelfX)) {
            return Coordinate { l_TestX - 1, l_TestY - 1 }; // lower left, upwards
        } else if ((l_TestY <= l_SelfY) && (l_TestX < l_SelfX)) {
            if ((l_TestX + 1) == l_SelfX) {
                return {}; // circumference completed
            } else {
                return Coordinate { l_TestX + 1, l_TestY - 1 }; // upper left, upwards
            } // else
        } else {
            assert(false);
            return {};
        } // else
    }
    
private:
    const Coordinate m_Self;
    const Coordinate m_Beacon;
    const std::uint64_t m_MHDistance;
};

class Coverage {
public:
    void AddSensor(const std::string& a_Line) {
        // Split "Sensor at x=2, y=18: closest beacon is at x=-2, y=15"
        auto l_RegEx = std::regex { "Sensor at x=(-?[[:digit:]]+), y=(-?[[:digit:]]+): closest beacon is at x=(-?[[:digit:]]+), y=(-?[[:digit:]]+)$" };
        auto l_SMatch = std::smatch {};
        if (std::regex_search(a_Line, l_SMatch, l_RegEx) && (l_SMatch.size() > 1)) {
            l_Sensors.emplace_back(Coordinate { std::atoi(std::string { l_SMatch[1] }.c_str()), std::atoi(std::string { l_SMatch[2] }.c_str()) },
                                   Coordinate { std::atoi(std::string { l_SMatch[3] }.c_str()), std::atoi(std::string { l_SMatch[4] }.c_str()) });
        } else {
            assert(false);
        } // else
    }
    
    [[nodiscard]] auto NoBeaconPossible(const Coordinate& a_TestCoord, bool a_bNeedSpace) const noexcept {
        for (const auto& l_Sensor: l_Sensors) {
            if (l_Sensor.HasBeaconAt(a_TestCoord)) {
                return a_bNeedSpace; // riddle A: false (do not count but stop), riddle B: true (treat as covered -> no space)
            } else if (l_Sensor.CoverageTest(a_TestCoord)) {
                return true; // covered, no space
            } else {
                // Not covered by this sensor, undecided
            } // else
        } // for
        
        return false; // not covered by any sensor
    }
    
    [[nodiscard]] auto SearchSpace(std::uint64_t a_SearchWidth) const noexcept {
        for (const auto& l_Sensor: l_Sensors) {
            auto l_oCurPosition = l_Sensor.StepAroundPerimeter({});
            while (l_oCurPosition.has_value()) {
                const auto& [l_X, l_Y] = l_oCurPosition.value();
                if ((l_X >= 0) && (l_X <= a_SearchWidth) && (l_Y >= 0) && (l_Y <= a_SearchWidth)) {
                    if (!NoBeaconPossible(Coordinate { l_X, l_Y }, true)) {
                        return Coordinate { l_X, l_Y };
                    } // if
                } // if
                
                l_oCurPosition = l_Sensor.StepAroundPerimeter(l_oCurPosition);
            } // while
        } // for
        
        return Coordinate{};
    }
    
private:
    std::vector<Sensor> l_Sensors;
};

void Play(std::string_view a_FileName, const std::int64_t a_LookAtY, std::uint64_t a_SearchWidth) {
    // Read sensor coverage information
    std::cout << std::endl << "Reading sensor coverage from file \"" << a_FileName << "\"" << std::endl;
    std::ifstream l_InputStream { std::string { a_FileName } };
    std::string l_Line;

    auto l_Coverage = Coverage{};
    while (std::getline(l_InputStream, l_Line)) {
        l_Coverage.AddSensor(l_Line);
    } // while
    
    //
    auto l_NoBeaconPositions { 0 };
    for (auto l_X = (a_LookAtY * (-5)); l_X < (a_LookAtY * 5); ++l_X) {
        if (l_Coverage.NoBeaconPossible(Coordinate { l_X, a_LookAtY }, false)) {
            ++l_NoBeaconPositions;
        } //
    } // for
    
    std::cout << "A: number of positions where the beacon cannot be = " << l_NoBeaconPositions << std::endl;
    const auto [l_X, l_Y] = l_Coverage.SearchSpace(a_SearchWidth);
    std::cout << "B: found beacon spot at " << l_X << ":" << l_Y << " with tuning frequency " << ((std::uint64_t { 4000000 } * l_X) + l_Y) << std::endl;
}

int main(int argc, char **argv) {
    Play("input-example.txt", 10, 20);
    Play("input.txt", 2000000, 4000000);
}
