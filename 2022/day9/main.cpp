#include <algorithm>
#include <array>
#include <cstddef>
#include <cassert>
#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <set>
#include <sstream>
#include <string_view>
#include <tuple>

class Coordinate {
public:
    // Make it storable in an set::set
    [[nodiscard]] auto operator<(const Coordinate& a_Other) const noexcept {
        return ((m_X < a_Other.m_X) || ((m_X == a_Other.m_X) && (m_Y < a_Other.m_Y)));
    }
    
    void MoveRight() noexcept { ++m_X; }
    void MoveLeft () noexcept { --m_X; }
    void MoveDown () noexcept { ++m_Y; }
    void MoveUp   () noexcept { --m_Y; }
    
    void Follow(const Coordinate& a_Head, std::set<Coordinate>* a_pVisitedCorrdinates) noexcept {
        auto l_XDistance = (a_Head.m_X - m_X);
        auto l_YDistance = (a_Head.m_Y - m_Y);
        if ((abs(l_XDistance) <= 1) && (abs(l_YDistance) <= 1)) {
            // Do not move
            return;
        } // if
        
        if (l_XDistance <= -1) {
            --m_X;
        } else if (l_XDistance >= 1) {
            ++m_X;
        } // else
        
        if (l_YDistance <= -1) {
            --m_Y;
        } else if (l_YDistance >= 1) {
            ++m_Y;
        } // else
        
        if (a_pVisitedCorrdinates) {
            a_pVisitedCorrdinates->insert(*this);
        } // if
    }
    
private:
    std::int32_t m_X = 0;
    std::int32_t m_Y = 0;
};

void Play(std::string_view a_FileName) {
    // Read the movements
    std::cout << std::endl << "Checking movement information from file \"" << a_FileName << "\"" << std::endl;
    std::ifstream l_InputStream { std::string { a_FileName } };
    std::string l_Line;
    
    auto l_Rope = std::array<Coordinate, 10>{};
    auto l_VisitedCoordinatesA = std::set<Coordinate>{ l_Rope[0] };
    auto l_VisitedCoordinatesB = std::set<Coordinate>{ l_Rope[0] };
    while (std::getline(l_InputStream, l_Line)) {
        // Parse one movement, but then only move the head of rope one step at a time
        const auto l_Steps = std::atoi(l_Line.substr(2, (l_Line.size() - 2)).c_str());
        for (auto l_Step = 0; l_Step < l_Steps; ++l_Step) {
            switch (l_Line[0]) {
                case 'R':
                    l_Rope[0].MoveRight();
                    break;
                case 'L':
                    l_Rope[0].MoveLeft();
                    break;
                case 'D':
                    l_Rope[0].MoveDown();                    
                    break;
                case 'U':
                    l_Rope[0].MoveUp();
                    break;
                default:
                    assert(false);
                    break;
            } // switch
            
            // Let the tail follow, knot by knot
            for (auto l_Knot = 1; l_Knot < l_Rope.size(); ++l_Knot) {
                auto l_pVisitedCoordinates = std::invoke([&l_Rope, &l_VisitedCoordinatesA, &l_VisitedCoordinatesB, l_Knot]() {
                    if (l_Knot == 1) {
                        return &l_VisitedCoordinatesA; // first riddle: track the knot following the head
                    } else if ((l_Knot + 1) == l_Rope.size()) {
                        return &l_VisitedCoordinatesB; // second riddle: track the last knot at the end of the rope
                    } else {
                        return static_cast<std::set<Coordinate>*>(nullptr);
                    } // else
                }); // std::invoke
                
                l_Rope[l_Knot].Follow(l_Rope[l_Knot - 1], l_pVisitedCoordinates);
            } // for
        } // for
    } // while
    
    std::cout << "Number of fields visited by the 1st knot following the head: " << l_VisitedCoordinatesA.size() << std::endl;
    std::cout << "Number of fields visited by the 9th knot following the head: " << l_VisitedCoordinatesB.size() << std::endl;
}

int main(int argc, char **argv) {
    Play("input-example.txt");
    Play("input-example2.txt");
    Play("input.txt");
}
