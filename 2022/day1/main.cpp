#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>

void ProcessA() {
    std::ifstream l_InputStream { "input.txt" };
    
    int l_Maximum = 0;
    int l_Current = 0;
    std::string l_Line;
    while (std::getline(l_InputStream, l_Line)) {
        if (l_Line.empty()) {
            if (l_Current > l_Maximum) {
                l_Maximum = l_Current;
            } // if
            
            l_Current = 0;
        } else {
            int l_LineVal = std::atoi(l_Line.c_str());
            l_Current += l_LineVal;
        } // else
    } // while
    
    std::cout << "Maximum = " << l_Maximum << std::endl;
}

void ProcessB() {
    std::ifstream l_InputStream { "input.txt" };
    
    std::vector<int> l_Results;
    std::string l_Line;
    int l_Current = 0;
    while (std::getline(l_InputStream, l_Line)) {
        if (l_Line.empty()) {
            l_Results.push_back(l_Current);
            l_Current = 0;
        } else {
            int l_LineVal = std::atoi(l_Line.c_str());
            l_Current += l_LineVal;
        } // else
    } // while
    
    std::sort(l_Results.begin(), l_Results.end());
    std::cout << "Total = " << std::accumulate(l_Results.end() - 3, l_Results.end(), int(0)) << std::endl;
}

int main(int argc, char **argv) {
    ProcessA();
    ProcessB();
}
