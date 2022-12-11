#include <algorithm>
#include <array>
#include <boost/tokenizer.hpp>
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

class MonkeyHerd;

class Monkey {
public:
    explicit Monkey(MonkeyHerd& a_MonkeyHerd): m_MonkeyHerd { a_MonkeyHerd } {}
    void Parse(std::string a_Line) {
        // Parse remainder of monkey
        if (a_Line.empty()) {
            // Ignore
        } else if (a_Line.substr(0, 18) == "  Starting items: ") {
            const auto l_ItemList = a_Line.substr(18, (a_Line.size() - 18));
            boost::char_separator<char> l_Separator { ", " };
            boost::tokenizer<boost::char_separator<char>> l_Tokenizer { l_ItemList, l_Separator };
            for (const auto &l_Token: l_Tokenizer) {
                m_ItemsList.push_back(std::atoi(l_Token.c_str()));
            } // for
        } else if (a_Line.substr(0, 28) == "  Operation: new = old * old") {
            m_Operator = [](std::uint64_t a_Old){ return (a_Old * a_Old); };
        } else if (a_Line.substr(0, 23) == "  Operation: new = old ") {
            const auto l_Modifier = std::atoi(a_Line.substr(25, (a_Line.size() - 25)).c_str());
            if (a_Line[23] == '+') {
                m_Operator = [l_Modifier](std::uint64_t a_Old){ return (a_Old + l_Modifier); };
            } else if (a_Line[23] == '*') {
                m_Operator = [l_Modifier](std::uint64_t a_Old){ return (a_Old * l_Modifier); };
            } else {
                assert(false);
            } // else
        } else if (a_Line.substr(0, 21) == "  Test: divisible by ") {
            m_Divisor = std::atoi(a_Line.substr(21, (a_Line.size() - 21)).c_str());
            m_Test = [this](std::uint64_t a_Value) { return static_cast<bool>((a_Value % m_Divisor) == 0); };
        } else if (a_Line.substr(0, 29) == "    If true: throw to monkey ") {
            m_OnTrueToMonkey = std::atoi(a_Line.substr(29, (a_Line.size() - 29)).c_str());
        } else if (a_Line.substr(0, 30) == "    If false: throw to monkey ") {
            m_OnFalseToMonkey = std::atoi(a_Line.substr(30, (a_Line.size() - 30)).c_str());
        } else {
            assert(false);
        } // else
    }
    
    void InspectItems(const std::function<std::uint64_t(std::uint64_t)>& a_WorrynessReducer, std::uint64_t a_PrimeProduct);
    void CatchItem(std::uint64_t a_Item) {
        m_ItemsList.push_back(a_Item);
    }
    
    [[nodiscard]] auto GetNbrOfInspectedItems() const noexcept { return m_NbrOfInspectedItems; }
    [[nodiscard]] auto GetDivisor() const noexcept { return m_Divisor; }
    
private:
    MonkeyHerd& m_MonkeyHerd;
    std::deque<std::uint64_t> m_ItemsList;
    std::function<std::uint64_t(std::uint64_t)> m_Operator;
    std::function<bool(std::uint64_t)> m_Test;
    std::uint64_t m_Divisor = 0;
    std::uint32_t m_OnTrueToMonkey = 0;
    std::uint32_t m_OnFalseToMonkey = 0;
    std::uint32_t m_NbrOfInspectedItems = 0;
};

class MonkeyHerd {
public:
    void Initialize(std::string a_Line) {
        if (a_Line.substr(0, 7) == "Monkey ") {
            l_Monkeys.emplace_back(Monkey { *this } );
        } else {
            l_Monkeys.back().Parse(a_Line);
        } // else
    }
    
    void ReadCompleted() {
        for (const auto& l_Monkey: l_Monkeys) {
            m_PrimeProduct *= l_Monkey.GetDivisor();
        } // for
    }
    
    void TriggerAllMonkeys(const std::function<std::uint64_t(std::uint64_t)>& a_WorrynessReducer) {
        for (auto& l_Monkey: l_Monkeys) {
            l_Monkey.InspectItems(a_WorrynessReducer, m_PrimeProduct);
        } // for
    }

    void ThrowItemToMonkey(std::uint64_t a_Item, std::uint32_t a_Monkey) {
        l_Monkeys[a_Monkey].CatchItem(a_Item);
    }
    
    [[nodiscard]] auto CalculateMonkeyBusiness() const noexcept {
        std::vector<std::uint64_t> l_Activities;
        for (const auto& l_Monkey: l_Monkeys) {
            l_Activities.push_back(l_Monkey.GetNbrOfInspectedItems());
        } // for
        
        std::sort(l_Activities.begin(), l_Activities.end());
        return (l_Activities[l_Activities.size() - 1] * l_Activities[l_Activities.size() - 2]);
    }
    
private:
    std::vector<Monkey> l_Monkeys;
    std::uint64_t m_PrimeProduct = 1;
};



void Monkey::InspectItems(const std::function<std::uint64_t(std::uint64_t)>& a_WorrynessReducer, std::uint64_t a_PrimeProduct) {
    // Inspect each item
    m_NbrOfInspectedItems += m_ItemsList.size();
    while (!m_ItemsList.empty()) {
        auto l_Item = m_ItemsList.front();
        m_ItemsList.pop_front();
        l_Item = m_Operator(l_Item);
        l_Item = a_WorrynessReducer(l_Item);
        l_Item %= a_PrimeProduct;
        if (m_Test(l_Item)) {
            m_MonkeyHerd.ThrowItemToMonkey(l_Item, m_OnTrueToMonkey);
        } else {
            m_MonkeyHerd.ThrowItemToMonkey(l_Item, m_OnFalseToMonkey);
        } // else
    } // while
}

void Play(std::string_view a_FileName) {
    // Read the monkey specs
    std::cout << std::endl << "Reading monkey specifications from file \"" << a_FileName << std::endl;
    std::ifstream l_InputStream { std::string { a_FileName } };
    std::string l_Line;

    auto l_MonkeyHerdA = MonkeyHerd{};
    auto l_MonkeyHerdB = MonkeyHerd{};
    while (std::getline(l_InputStream, l_Line)) {
        l_MonkeyHerdA.Initialize(l_Line);
        l_MonkeyHerdB.Initialize(l_Line);
    } // while
    
    l_MonkeyHerdA.ReadCompleted(); // Update cache
    l_MonkeyHerdB.ReadCompleted(); // Update cache
    
    for (auto l_Round = 0; l_Round < 20; ++l_Round) {
        l_MonkeyHerdA.TriggerAllMonkeys([](std::uint64_t a_Item){ return (a_Item / 3); });
    } // for
    
    for (auto l_Round = 0; l_Round < 10000; ++l_Round) {
        l_MonkeyHerdB.TriggerAllMonkeys([](std::uint64_t a_Item){ return a_Item; });
    } // for
    
    std::cout << "Monkey business A = " << l_MonkeyHerdA.CalculateMonkeyBusiness() << std::endl;
    std::cout << "Monkey business B = " << l_MonkeyHerdB.CalculateMonkeyBusiness() << std::endl;
}

int main(int argc, char **argv) {
    Play("input-example.txt");
    Play("input.txt");
}
