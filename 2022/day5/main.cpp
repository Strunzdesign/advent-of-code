#include <algorithm>
#include <array>
#include <cassert>
#include <deque>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string_view>

int main(int argc, char **argv) {
    // Read the game
    auto l_FieldA = std::array<std::deque<char>, 9> {};
    auto l_FieldB = std::array<std::deque<char>, 9> {};
    
    std::ifstream l_InputStream { "input.txt" };
    std::string l_Line;
    bool l_ReadField = true;
    while (std::getline(l_InputStream, l_Line)) {
        if (l_Line.empty()) {
            // Done with reading the field, now the moves
            l_ReadField = false;
            l_FieldB = l_FieldA;
        } else if (l_ReadField) {
            // Read the field
            for (int l_Column = 0; l_Column < l_FieldA.size(); ++l_Column) {
                const auto l_Element = l_Line[1 + (l_Column * 4)];
                if ((l_Element >= 'A') && (l_Element <= 'Z')) {
                    // Valid element
                    l_FieldA[l_Column].push_front(l_Element);
                } // if
            } // for
        } else {
            // Read the moves: "move 2 from 2 to 7"
            const auto l_Move = std::atoi(l_Line.substr(5, (l_Line.find(" from") - 5)).c_str());
            const auto l_From = std::atoi(l_Line.substr((l_Line.find(" from ") + 6), ((l_Line.find(" to")) - (l_Line.find(" from ") + 6))).c_str());
            const auto l_To   = std::atoi(l_Line.substr((l_Line.find(" to ") + 4), l_Line.size() - (l_Line.find(" to ") + 4)).c_str());
            
            auto l_SplicerB = std::deque<char>{};
            for (auto l_Run = 0; l_Run < l_Move; ++l_Run) {
                assert(l_FieldA[l_From - 1].empty() == false);
                l_FieldA[l_To - 1].push_back(l_FieldA[l_From - 1].back());
                l_FieldA[l_From - 1].pop_back();
                
                assert(l_FieldB[l_From - 1].empty() == false);
                l_SplicerB.push_front(l_FieldB[l_From - 1].back());
                l_FieldB[l_From - 1].pop_back();
            } // for
            
            l_FieldB[l_To - 1].insert(l_FieldB[l_To - 1].end(), l_SplicerB.begin(), l_SplicerB.end());
        } // else
    } // while
    
    // Print the tops
    for (const auto& l_Column: l_FieldA) {
        std::cout << l_Column.back();
    } // for
    
    std::cout << std::endl;
    for (const auto& l_Column: l_FieldB) {
        std::cout << l_Column.back();
    } // for
    
    std::cout << std::endl;
}
