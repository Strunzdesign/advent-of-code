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

class Folder {
public:
    Folder(Folder* a_pParent): m_pParent { a_pParent } {}
    void AddFolder(std::string_view a_Folder) {
        assert(m_SubFolders.find(std::string { a_Folder } ) == m_SubFolders.end());
        m_SubFolders[std::string { a_Folder } ] = std::make_unique<Folder>(this);
    }
    
    void AddFile(std::uint64_t a_FileSize, std::string_view a_FileName) {
        assert(m_Files.find(std::string { a_FileName } ) == m_Files.end());
        m_Files[std::string { a_FileName } ] = a_FileSize;
    }
    
    [[nodiscard]] auto* ChangeFolder(std::string_view a_Folder) {
        const auto l_ChildNodeIt = m_SubFolders.find(std::string { a_Folder } );
        assert(l_ChildNodeIt != m_SubFolders.end());
        return l_ChildNodeIt->second.get();
    }
    
    [[nodiscard]] auto* Up() const {
        assert(m_pParent);
        return m_pParent;
    }
    
    // Evaluation
    [[nodiscard]] auto Calculate(const std::uint64_t a_MaxBytes, const std::uint64_t a_RequiredSpace) const -> std::tuple<std::uint64_t, std::uint64_t, std::uint64_t> {
        // Returns all accumulated sizes of all subfolders + real size        
        // Depth search
        std::uint64_t l_RealSize = 0;
        std::uint64_t l_MatchSize = 0;
        std::uint64_t l_BestDirectorySize = 0; // second riddle
        for (const auto& l_SubFolders: m_SubFolders) {
            auto [l_RealSizeSingle, l_MatchSizeSingle, l_BestDirectorySizeSingle] = l_SubFolders.second->Calculate(a_MaxBytes, a_RequiredSpace);
            l_RealSize += l_RealSizeSingle;
            l_MatchSize += l_MatchSizeSingle;
            
            // Check for second riddle
            if ((l_BestDirectorySizeSingle >= a_RequiredSpace) && ((l_BestDirectorySize == 0) || (l_BestDirectorySizeSingle < l_BestDirectorySize))) {
                l_BestDirectorySize = l_BestDirectorySizeSingle;
            } // if
        } // for
        
        // Add all local files
        for (const auto& l_File: m_Files) {
            l_RealSize += l_File.second;
        } // for
        
        // Check for first riddle
        if (l_RealSize <= a_MaxBytes) {
            l_MatchSize += l_RealSize;
        } // if
        
        // Check for second riddle
        if ((l_RealSize >= a_RequiredSpace) && ((l_BestDirectorySize == 0) || (l_RealSize < l_BestDirectorySize))) {
            l_BestDirectorySize = l_RealSize;
        } // if

        return std::make_tuple(l_RealSize, l_MatchSize, l_BestDirectorySize);
    }
    
private:
    Folder* m_pParent = nullptr;
    std::map<std::string, std::unique_ptr<Folder>> m_SubFolders;
    std::map<std::string, std::uint64_t> m_Files;
};

[[nodiscard]] auto ProcessCommand(Folder* a_pCurrentNode, std::string_view a_Command) {
    if ((a_Command == "$ cd /") || (a_Command == "$ ls")) {
        // Ignore
    } else if (a_Command == "$ cd ..") {
        // Go up in hierarchy
        return a_pCurrentNode->Up();
    } else if (a_Command.substr(0, 4) == "$ cd") {
        // Dive into folder
        return a_pCurrentNode->ChangeFolder(a_Command.substr(5, (a_Command.size() - 5)));
    } else if (a_Command.substr(0, 3) == "dir") {
        // Add a subfolder
        a_pCurrentNode->AddFolder(a_Command.substr(4, (a_Command.size() - 4)));
    } else {
        // Add a file
        auto l_FileSpec = std::stringstream { std::string { a_Command } };
        std::string l_FileSize;
        auto l_ParseResult1 = static_cast<bool>(std::getline(l_FileSpec, l_FileSize, ' '));
        assert(l_ParseResult1);
        std::string l_FileName;
        auto l_ParseResult2 = static_cast<bool>(std::getline(l_FileSpec, l_FileName, ' '));
        assert(l_ParseResult2);
        a_pCurrentNode->AddFile(std::atoi(l_FileSize.c_str()), l_FileName);
    } // else
    
    return a_pCurrentNode;
}

void Play(std::string_view a_FileName) {
    // Read all commands and outputs to build the file hierarchy
    std::cout << "Playing command sequence from file \"" << a_FileName << "\"" << std::endl;
    std::ifstream l_InputStream { std::string { a_FileName } };
    std::string l_Line;
    auto l_pRootNode = std::make_unique<Folder>(nullptr);
    Folder* l_pCurrentNode = l_pRootNode.get();
    while (std::getline(l_InputStream, l_Line)) {
        l_pCurrentNode = ProcessCommand(l_pCurrentNode, l_Line);
    } // while
    
    // First riddle: process the file hierarchy
    auto [l_RealSize, l_MatchSize, l_BestDirectorySize] = l_pRootNode->Calculate(100000, 0);
    std::cout << "Size of all files: " << l_RealSize << std::endl;;
    std::cout << "Accumulated bytes of folders with leq 100000 bytes: " << l_MatchSize << std::endl;;
    
    // Second riddle: traverse again, as we know the total size now
    auto l_AdditionalBytesRequired = (30000000 - (70000000 - l_RealSize));
    std::cout << "Additional amount of bytes required to be freed: " << l_AdditionalBytesRequired << std::endl;;
    auto [l_RealSize2, l_MatchSize2, l_BestDirectorySize2] = l_pRootNode->Calculate(0, l_AdditionalBytesRequired);
    std::cout << "Best size to delete: " << l_BestDirectorySize2 << std::endl << std::endl;
}

int main(int argc, char **argv) {
    Play("input-example.txt");
    Play("input.txt");
}
