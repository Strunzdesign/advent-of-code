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
    // Read the depth values from the sonar file
    std::cout << std::endl << "Reading sonar depth measurements from file \"" << a_FileName << "\"" << std::endl;
    std::ifstream l_InputStream { std::string { a_FileName } };
    std::string l_Line;

    auto l_FilterA = boost::circular_buffer<std::uint32_t>(1);
    auto l_FilterB = boost::circular_buffer<std::uint32_t>(3);
    std::uint32_t l_DepthIncreasesA = 0;
    std::uint32_t l_DepthIncreasesB = 0;
    std::optional<std::uint32_t> l_oLastDepthA;
    std::optional<std::uint32_t> l_oLastDepthB;
    while (std::getline(l_InputStream, l_Line)) {
        const auto l_CurrentDepth { std::atoi(l_Line.c_str()) };
        l_FilterA.push_back(l_CurrentDepth);
        if (l_FilterA.capacity() == l_FilterA.size()) {
            const auto l_FilteredDepth = std::accumulate(l_FilterA.begin(), l_FilterA.end(), std::uint32_t { 0 } );
            if (l_oLastDepthA.has_value() && (l_FilteredDepth > l_oLastDepthA.value())) {
                ++l_DepthIncreasesA;
            } // if
            
            l_oLastDepthA = l_FilteredDepth;
        } // if
        
        l_FilterB.push_back(l_CurrentDepth);
        if (l_FilterB.capacity() == l_FilterB.size()) {
            const auto l_FilteredDepth = std::accumulate(l_FilterB.begin(), l_FilterB.end(), std::uint32_t { 0 } );
            if (l_oLastDepthB.has_value() && (l_FilteredDepth > l_oLastDepthB.value())) {
                ++l_DepthIncreasesB;
            } // if
            
            l_oLastDepthB = l_FilteredDepth;
        } // if
    } // while
    
    std::cout << "Number of depth increases = " << l_DepthIncreasesA << std::endl;
    std::cout << "Number of filtered depth increases = " << l_DepthIncreasesB << std::endl;
}

int main(int argc, char **argv) {
    Play("input-example.txt");
    Play("input.txt");
}
