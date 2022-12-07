#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string_view>
#include <vector>

class Range {
public:
    explicit Range(std::string a_RangeString) {
        auto l_Input = std::stringstream { a_RangeString };
        std::string l_Element1;
        auto l_ParseResult1 = static_cast<bool>(std::getline(l_Input, l_Element1, '-'));
        assert(l_ParseResult1);
        std::string l_Element2;
        auto l_ParseResult2 = static_cast<bool>(std::getline(l_Input, l_Element2, '-'));
        assert(l_ParseResult2);
        m_Lower = std::atoi(l_Element1.c_str());
        m_Upper = std::atoi(l_Element2.c_str());
    }
    
    [[nodiscard]] auto GetLower() const noexcept { return m_Lower; }
    [[nodiscard]] auto GetUpper() const noexcept { return m_Upper; }
    
private:
    int m_Lower;
    int m_Upper;
};

[[nodiscard]] bool inline FullyContains(const Range& a_Outer, const Range& a_Inner) {
    return ((a_Outer.GetLower() <= a_Inner.GetLower()) && (a_Outer.GetUpper() >= a_Inner.GetUpper()));
}

[[nodiscard]] bool inline Overlaps(const Range& a_First, const Range& a_Second) {
    return (!((a_First.GetUpper() < a_Second.GetLower()) || (a_First.GetLower() > a_Second.GetUpper())));
}

int main(int argc, char **argv) {
    std::ifstream l_InputStream { "input.txt" };

    int l_ContainingSets = 0;
    int l_OverlappingSets = 0;

    std::string l_Line;
    while (std::getline(l_InputStream, l_Line)) {
        auto l_Input = std::stringstream { l_Line };
        std::string l_Element1;
        auto l_ParseResult1 = static_cast<bool>(std::getline(l_Input, l_Element1, ','));
        assert(l_ParseResult1);
        std::string l_Element2;
        auto l_ParseResult2 = static_cast<bool>(std::getline(l_Input, l_Element2, ','));
        assert(l_ParseResult2);

        // Extract range
        const auto l_Elve1 = Range { l_Element1 };
        const auto l_Elve2 = Range { l_Element2 };

        if (FullyContains(l_Elve1, l_Elve2) || FullyContains(l_Elve2, l_Elve1)) {
            ++l_ContainingSets;
        } // if
        
        if (Overlaps(l_Elve1, l_Elve2)) {
            ++l_OverlappingSets;
        } // if
    } // while
    
    std::cout << "Amount of containing sets = " << l_ContainingSets << std::endl;
    std::cout << "Amount of overlapping sets = " << l_OverlappingSets << std::endl;
}
