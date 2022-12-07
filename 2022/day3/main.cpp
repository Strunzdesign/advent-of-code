#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <string_view>
#include <vector>

void ProcessA() {
    std::ifstream l_InputStream { "input.txt" };
    
    int l_PriorityAcc = 0;
    std::string l_Line;
    while (std::getline(l_InputStream, l_Line)) {
        assert((l_Line.size() % 2) == 0);
        
        const auto l_CompA = l_Line.substr(0, (l_Line.size() >> 1));
        const auto l_CompB = l_Line.substr((l_Line.size() >> 1), l_Line.size());
        
        std::set<char> l_ItemsA;
        for (const char l_Letter: l_CompA) {
            l_ItemsA.insert(l_Letter);
        } // for
        
        std::set<char> l_ItemsB;
        for (const char l_Letter: l_CompB) {
            l_ItemsB.insert(l_Letter);
        } // for
        
        char l_Result = '-';
        for (const char l_ItemA: l_ItemsA) {
            if (l_ItemsB.count(l_ItemA) == 1) {
                assert(l_Result == '-');
                l_Result = l_ItemA;
            }
        }
        
        // Calculate resulting priority
        assert(l_Result != '-');
        if ((l_Result >= 'a') && (l_Result <= 'z')) {
            l_PriorityAcc += (int(l_Result) - 96);
        } else if ((l_Result >= 'A') && (l_Result <= 'Z')) {
            l_PriorityAcc += (int(l_Result) - 64 + 26);
        } else {
            assert(false);
        } // else
    } // while
    
    std::cout << "Solution for riddle A = " << l_PriorityAcc << std::endl;
}

void ProcessB() {
    std::ifstream l_InputStream { "input.txt" };
    
    int l_PriorityAcc = 0;
    std::string l_Line;
    while (std::getline(l_InputStream, l_Line)) {
        assert((l_Line.size() % 2) == 0);
        std::set<char> l_ItemsA;
        for (const char l_Letter: l_Line) {
            l_ItemsA.insert(l_Letter);
        } // for
        
        const auto l_Read2 = static_cast<bool>(std::getline(l_InputStream, l_Line));
        assert(l_Read2);
        assert((l_Line.size() % 2) == 0);
        std::set<char> l_ItemsB;
        for (const char l_Letter: l_Line) {
            l_ItemsB.insert(l_Letter);
        } // for
        
        const auto l_Read3 = static_cast<bool>(std::getline(l_InputStream, l_Line));
        assert(l_Read3);
        assert((l_Line.size() % 2) == 0);
        std::set<char> l_ItemsC;
        for (const char l_Letter: l_Line) {
            l_ItemsC.insert(l_Letter);
        } // for
        
        std::set<char> l_ItemsAB;
        for (const char l_ItemA: l_ItemsA) {
            if (l_ItemsB.count(l_ItemA) == 1) {
                l_ItemsAB.insert(l_ItemA);
            } // if
        } // for
        
        std::set<char> l_ItemsABC;
        for (const char l_ItemAB: l_ItemsAB) {
            if (l_ItemsC.count(l_ItemAB) == 1) {
                l_ItemsABC.insert(l_ItemAB);
            } // if
        } // for
        
        // Calculate resulting priority
        auto l_Result = *(l_ItemsABC.begin());
        if ((l_Result >= 'a') && (l_Result <= 'z')) {
            l_PriorityAcc += (int(l_Result) - 96);
        } else if ((l_Result >= 'A') && (l_Result <= 'Z')) {
            l_PriorityAcc += (int(l_Result) - 64 + 26);
        } else {
            assert(false);
        } // else
    } // while
    
    std::cout << "Solution for riddle B = " << l_PriorityAcc << std::endl;
}

int main(int argc, char **argv) {
    ProcessA();
    ProcessB();
}
