#include <algorithm>
#include <array>
#include <cstddef>
#include <cassert>
#include <deque>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <sstream>
#include <string_view>

template<std::size_t SIZE>
[[nodiscard]] auto IsMarker(std::string_view a_Substream) {
    assert(a_Substream.size() == SIZE);
    auto l_SeenChars = std::set<char>{};
    for (const auto l_Char: a_Substream) {
        l_SeenChars.insert(l_Char);
    } // for
    
    return (l_SeenChars.size() == SIZE);
}

template<std::size_t SIZE> void FindMarker(std::string_view a_Stream) {
    for (auto l_Offset = 0; l_Offset < (a_Stream.size() - SIZE); ++l_Offset) {
        if (IsMarker<SIZE>(a_Stream.substr(l_Offset, SIZE))) {
            std::cout << "Found marker \"" << a_Stream.substr(l_Offset, SIZE) << "\" at Offset " << l_Offset << ", after reading " << (l_Offset + SIZE) << " chars" << std::endl;
            break;
        } // if
    } // for
}

int main(int argc, char **argv) {
    std::ifstream l_InputStream { "input.txt" };
    std::string l_OnLineData;
    const auto l_ReadResult = static_cast<bool>(std::getline(l_InputStream, l_OnLineData));
    assert(l_ReadResult);
    
    FindMarker<4>(l_OnLineData);
    FindMarker<14>(l_OnLineData);
}
