#include <algorithm>
#include <array>
#include <bitset>
#include <boost/circular_buffer.hpp>
#include <cstddef>
#include <cassert>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <set>
#include <sstream>
#include <string_view>
#include <tuple>

class Diagnostic {
public:
    using BitStorage = std::bitset<12>;
    
    void AddBits(std::string a_Bits) {
        assert(a_Bits.size());
        assert(!m_NbrOfBits || (m_NbrOfBits == a_Bits.size()));
        m_Data.push_back(BitStorage { a_Bits });
        m_NbrOfBits = a_Bits.size();
    }
    
    [[nodiscard]] auto CalculateRate(const std::function<bool(std::uint32_t a_NbrOf1, std::uint32_t a_NbrOf0)>& a_Comparator) const noexcept {
        // Riddle A: gamma and epsilon rate
        std::uint64_t l_Rate = 0;
        for (std::int32_t l_Exp = (m_NbrOfBits - 1); l_Exp >= 0; --l_Exp) {
            auto [l_NbrOf1, l_NbrOf0] = CalculateNbrOfBits(m_Data, l_Exp);            
            if (a_Comparator(l_NbrOf1, l_NbrOf0)) {
                l_Rate += (1 << l_Exp);
            } // if
        } // for
        
        return l_Rate;
    }
    
    [[nodiscard]] auto CalculateRating(const std::function<bool(std::uint32_t a_NbrOf1, std::uint32_t a_NbrOf0)>& a_Comparator) const noexcept {
        // Riddle B: oxygen aus CO2 scrubber rating
        auto l_Data { m_Data };
        for (std::int32_t l_Exp = (m_NbrOfBits - 1); l_Exp >= 0; --l_Exp) {
            auto [l_NbrOf1, l_NbrOf0] = CalculateNbrOfBits(l_Data, l_Exp);
            auto l_DataTemp { l_Data };
            for (const auto& l_Bits: l_Data) {
                if (l_Bits.test(l_Exp) != a_Comparator(l_NbrOf1, l_NbrOf0)) {
                    l_DataTemp.remove(l_Bits);
                    if (l_DataTemp.size() == 1) {
                        return l_DataTemp.begin()->to_ulong();
                    } // if
                } // if
            } // for
            
            l_Data = l_DataTemp;
        } // for
        
        assert(false);
    }
    
private:
    auto CalculateNbrOfBits(const std::list<BitStorage>& a_Data, std::int32_t a_Exp) const noexcept -> std::pair<std::uint32_t, std::uint32_t> {
        std::uint32_t l_NbrOf1 = 0;
        std::uint32_t l_NbrOf0 = 0;
        for (const auto& l_Bits: a_Data) {
            if (l_Bits.test(a_Exp)) {
                ++l_NbrOf1;
            } else {
                ++l_NbrOf0;
            } // else
        } // for
        
        return std::make_pair(l_NbrOf1, l_NbrOf0);
    }
    
    std::list<BitStorage> m_Data;
    std::uint32_t m_NbrOfBits = 0;
};

void Play(std::string_view a_FileName) {
    // Read the diagnostic data from the file
    std::cout << std::endl << "Reading diagnostic data from file \"" << a_FileName << "\"" << std::endl;
    std::ifstream l_InputStream { std::string { a_FileName } };
    std::string l_Line;
    
    auto l_Diagnostic = Diagnostic{};
    while (std::getline(l_InputStream, l_Line)) {
        l_Diagnostic.AddBits(l_Line);
    } // while
    
    const auto l_GammaRate = l_Diagnostic.CalculateRate([](std::uint32_t a_NbrOf1, std::uint32_t a_NbrOf0){ return a_NbrOf1 > a_NbrOf0; });
    const auto l_EpsilonRate = l_Diagnostic.CalculateRate([](std::uint32_t a_NbrOf1, std::uint32_t a_NbrOf0){ return a_NbrOf1 <= a_NbrOf0; });
    const auto l_OxygenRating = l_Diagnostic.CalculateRating([](std::uint32_t a_NbrOf1, std::uint32_t a_NbrOf0){ return a_NbrOf1 >= a_NbrOf0; });
    const auto l_ScrubberRatig = l_Diagnostic.CalculateRating([](std::uint32_t a_NbrOf1, std::uint32_t a_NbrOf0){ return a_NbrOf1 < a_NbrOf0; });
    std::cout << "Gamma rate = " << l_GammaRate << ", epsilon rate = " << l_EpsilonRate << ", power = " << (l_GammaRate * l_EpsilonRate) << std::endl;
    std::cout << "Oxygen rating = " << l_OxygenRating << ", scrubber rating = " << l_ScrubberRatig << ", life support rating = " << (l_OxygenRating * l_ScrubberRatig) << std::endl;
}

int main(int argc, char **argv) {
    Play("input-example.txt");
    Play("input.txt");
}
