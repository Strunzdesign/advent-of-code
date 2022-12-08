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
#include <set>
#include <sstream>
#include <string_view>
#include <tuple>

using T_Forest = std::vector<std::vector<std::tuple<std::size_t, bool, std::size_t>>>; // height, visibility, scenic score

void CalculateVisibility(T_Forest& a_Forest) {
    // Mark all trees as visible by looking from all sides
    const auto l_ForestHeight = a_Forest.size();
    const auto l_ForestWidth  = a_Forest[0].size();    
    const auto l_CheckTree = [&a_Forest, l_ForestHeight, l_ForestWidth](std::size_t a_Row, std::size_t a_Column, std::size_t a_HighestTree) {
        auto& [l_CurrentHeight, l_bCurrentVisibility, l_CurrentScenicScore] = a_Forest[a_Row][a_Column];
        if ((a_Column == 0) || (a_Column == (l_ForestWidth - 1)) || (a_Row == 0) || (a_Row == (l_ForestHeight - 1)) || (a_HighestTree < l_CurrentHeight)) {
            l_bCurrentVisibility = true;
        } // if
        
        return std::max(a_HighestTree, l_CurrentHeight); // new highest tree seen so far
    }; // lambda
    
    // check top to down and down to up
    for (auto l_Column = 0; l_Column < l_ForestWidth; ++l_Column) {
        std::size_t l_HighestTreeUD = 0;
        std::size_t l_HighestTreeDU = 0;
        for (auto l_Row = 0; l_Row < l_ForestHeight; ++l_Row) {
            l_HighestTreeUD = l_CheckTree(l_Row, l_Column, l_HighestTreeUD);
            l_HighestTreeDU = l_CheckTree((l_ForestHeight - l_Row - 1), (l_ForestWidth - l_Column - 1), l_HighestTreeDU);
        } // for
    } // for

    // check left to right and right to left
    for (auto l_Row = 0; l_Row < l_ForestHeight; ++l_Row) {
        std::size_t l_HighestTreeLR = 0;
        std::size_t l_HighestTreeRL = 0;
        for (auto l_Column = 0; l_Column < l_ForestWidth; ++l_Column) {
            l_HighestTreeLR = l_CheckTree(l_Row, l_Column, l_HighestTreeLR);
            l_HighestTreeRL = l_CheckTree((l_ForestHeight - l_Row - 1), (l_ForestWidth - l_Column - 1), l_HighestTreeRL);
        } // for
    } // for
}

void CalculateScenicView(T_Forest& a_Forest) {
    // Check each tree of the forest
    const auto l_ForestHeight = a_Forest.size();
    const auto l_ForestWidth  = a_Forest[0].size();    
    for (auto l_Column = 0; l_Column < l_ForestWidth; ++l_Column) {
        for (auto l_Row = 0; l_Row < l_ForestHeight; ++l_Row) {
            // Now we have a tree to check at a given coordinate
            auto& [l_CurrentHeight, l_bCurrentVisibility, l_CurrentScenicScore] = a_Forest[l_Row][l_Column];
            
            auto l_ScenicScoreR = 0; // Check to the right
            for (auto l_ColumnTest = (l_Column + 1); l_ColumnTest < l_ForestWidth; ++l_ColumnTest) {
                ++l_ScenicScoreR;
                auto& [l_CurrentHeightT, l_bCurrentVisibilityT, l_CurrentScenicScoreT] = a_Forest[l_Row][l_ColumnTest];
                if (l_CurrentHeightT >= l_CurrentHeight) {
                    break;
                } // if
            } // for
            
            auto l_ScenicScoreL = 0; // Check to the left
            for (auto l_ColumnTest = (l_Column - 1); l_ColumnTest >= 0; --l_ColumnTest) {
                ++l_ScenicScoreL;
                auto& [l_CurrentHeightT, l_bCurrentVisibilityT, l_CurrentScenicScoreT] = a_Forest[l_Row][l_ColumnTest];
                if (l_CurrentHeightT >= l_CurrentHeight) {
                    break;
                } // if
            } // for
            
            auto l_ScenicScoreU = 0; // Check upwards
            for (auto l_RowTest = (l_Row - 1); l_RowTest >= 0; --l_RowTest) {
                ++l_ScenicScoreU;
                auto& [l_CurrentHeightT, l_bCurrentVisibilityT, l_CurrentScenicScoreT] = a_Forest[l_RowTest][l_Column];
                if (l_CurrentHeightT >= l_CurrentHeight) {
                    break;
                } // if
            } // for
            
            auto l_ScenicScoreD = 0; // Check downwards
            for (auto l_RowTest = (l_Row + 1); l_RowTest < l_ForestHeight; ++l_RowTest) {
                ++l_ScenicScoreD;
                auto& [l_CurrentHeightT, l_bCurrentVisibilityT, l_CurrentScenicScoreT] = a_Forest[l_RowTest][l_Column];
                if (l_CurrentHeightT >= l_CurrentHeight) {
                    break;
                } // if
            } // for
            
            l_CurrentScenicScore = (l_ScenicScoreR * l_ScenicScoreL * l_ScenicScoreU * l_ScenicScoreD);
        } // for
    } // for
}

void Play(std::string_view a_FileName) {
    // Read the forest
    std::cout << std::endl << "Checking forest from file \"" << a_FileName << "\"" << std::endl;
    auto l_Forest = T_Forest {}; // height + visibility
    std::ifstream l_InputStream { std::string { a_FileName } };
    std::string l_Line;
    while (std::getline(l_InputStream, l_Line)) {
        auto& l_NewRow = l_Forest.emplace_back(std::vector<std::tuple<std::size_t, bool, std::size_t>> {});
        for (const auto l_Char: l_Line) {
            l_NewRow.push_back(std::make_tuple((l_Char - '0'), false, 0 )); // Every tree is invisible by default with a scenic score of 0
        } // for
    } // while
    
    CalculateVisibility(l_Forest);
    CalculateScenicView(l_Forest);

    // Count all invisible trees and check best scenic score
    std::size_t l_Trees = 0;
    std::size_t l_VisibleTrees = 0;
    std::size_t l_BestScenicScore = 0;
    for (const auto& l_Row: l_Forest) {
        for (const auto& l_Tree: l_Row) {
            ++l_Trees;
            const auto& [l_Height, l_bVisibility, l_ScenicScore] = l_Tree;
            l_BestScenicScore = std::max(l_BestScenicScore, l_ScenicScore); // Second riddle
            if (l_bVisibility) {
                ++l_VisibleTrees;
            } // if
        } // for
    } // for

    std::cout << "At total, " << l_VisibleTrees << " trees of " << l_Trees << " are visible" << std::endl;
    std::cout << "Best scenic score = " << l_BestScenicScore << std::endl;
}

int main(int argc, char **argv) {
    Play("input-example.txt"); // Wrong: 16, too low
    Play("input.txt"); // Wrong: 404, too low. 1681 too high
}
