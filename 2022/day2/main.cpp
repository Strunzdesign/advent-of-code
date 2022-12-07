#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string_view>
#include <vector>

enum class Item: std::uint8_t {
    Rock = 1,
    Paper = 2,
    Scissors = 3
};

enum class Reward: std::uint8_t {
    Lose = 0,
    Draw = 3,
    Win = 6
};

[[nodiscard]] auto inline CalcWinReward(const Item& a_MyItem, const Item& a_HisItem) {
    switch (a_MyItem) {
        case Item::Rock:
            switch (a_HisItem) {
                case Item::Rock:
                    return Reward::Draw;
                case Item::Paper:
                    return Reward::Lose;
                case Item::Scissors:
                    return Reward::Win;
            } // switch
        case Item::Paper:
            switch (a_HisItem) {
                case Item::Rock:
                    return Reward::Win;
                case Item::Paper:
                    return Reward::Draw;
                case Item::Scissors:
                    return Reward::Lose;
            } // switch
        case Item::Scissors:
            switch (a_HisItem) {
                case Item::Rock:
                    return Reward::Lose;
                case Item::Paper:
                    return Reward::Win;
                case Item::Scissors:
                    return Reward::Draw;
            } // switch
        default: {
            std::cout << "Logic Error" << std::endl;
            exit(0);
        }
    } // switch
}

[[nodiscard]] auto inline CalcMyTurn(const Item& a_HisItem, const Reward& a_Reward) {
    switch (a_HisItem) {
        case Item::Rock:
            switch (a_Reward) {
                case Reward::Lose:
                    return Item::Scissors;
                case Reward::Draw:
                    return Item::Rock;
                case Reward::Win:
                    return Item::Paper;
            } // switch
        case Item::Paper:
            switch (a_Reward) {
                case Reward::Lose:
                    return Item::Rock;
                case Reward::Draw:
                    return Item::Paper;
                case Reward::Win:
                    return Item::Scissors;
            } // switch
        case Item::Scissors:
            switch (a_Reward) {
                case Reward::Lose:
                    return Item::Paper;
                case Reward::Draw:
                    return Item::Scissors;
                case Reward::Win:
                    return Item::Rock;
            } // switch
        default: {
            std::cout << "Logic Error" << std::endl;
            exit(0);
        }
    } // switch
}

class PlayGameA {
public:
    PlayGameA(std::string_view a_Line) {
        switch (a_Line[0]) {
            case 'a':
            case 'A':
                m_HisSelection = Item::Rock;
                break;
            case 'b':
            case 'B':
                m_HisSelection = Item::Paper;
                break;
            case 'c':
            case 'C':
                m_HisSelection = Item::Scissors;
                break;
            default:
                std::cout << "Input Error: " << a_Line << std::endl;
                exit(0);
        } // switch
        
        switch (a_Line[2]) {
            case 'x':
            case 'X':
                m_MySelection = Item::Rock;
                break;
            case 'y':
            case 'Y':
                m_MySelection = Item::Paper;
                break;
            case 'z':
            case 'Z':
                m_MySelection = Item::Scissors;
                break;
            default:
                std::cout << "Input Error: " << a_Line << std::endl;
                exit(0);
        } // switch
    }
    
    [[nodiscard]] auto CalcReward() const noexcept {
        return (int(m_MySelection) + int(CalcWinReward(m_MySelection, m_HisSelection)));
    }
    
private:
    Item m_MySelection;
    Item m_HisSelection;
};

class PlayGameB {
public:
    PlayGameB(std::string_view a_Line) {
        switch (a_Line[0]) {
            case 'a':
            case 'A':
                m_HisSelection = Item::Rock;
                break;
            case 'b':
            case 'B':
                m_HisSelection = Item::Paper;
                break;
            case 'c':
            case 'C':
                m_HisSelection = Item::Scissors;
                break;
            default:
                std::cout << "Input Error: " << a_Line << std::endl;
                exit(0);
        } // switch
        
        switch (a_Line[2]) {
            case 'x':
            case 'X':
                m_Reward = Reward::Lose;
                break;
            case 'y':
            case 'Y':
                m_Reward = Reward::Draw;
                break;
            case 'z':
            case 'Z':
                m_Reward = Reward::Win;
                break;
            default:
                std::cout << "Input Error: " << a_Line << std::endl;
                exit(0);
        } // switch
    }
    
    [[nodiscard]] auto CalcReward() const noexcept {
        const auto l_MySelection = CalcMyTurn(m_HisSelection, m_Reward);
        return (int(l_MySelection) + int(CalcWinReward(l_MySelection, m_HisSelection)));
    }
    
private:
    Item m_HisSelection;
    Reward m_Reward;
};

int main(int argc, char **argv) {
    std::ifstream l_InputStream { "input.txt" };
    
    int l_TotalGameScoreA = 0;
    int l_TotalGameScoreB = 0;
    std::string l_Line;
    while (std::getline(l_InputStream, l_Line)) {
        auto l_GameScoreA = PlayGameA { l_Line };
        auto l_GameScoreB = PlayGameB { l_Line };
        l_TotalGameScoreA += l_GameScoreA.CalcReward();
        l_TotalGameScoreB += l_GameScoreB.CalcReward();
    }
    
    std::cout << "Total reward riddle A = " << l_TotalGameScoreA << std::endl;
    std::cout << "Total reward riddle B = " << l_TotalGameScoreB << std::endl;
}
