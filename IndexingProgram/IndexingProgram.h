#pragma once

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>
#include <set>
#include <algorithm>
#include <map>

class IndexingManager {
public:
    IndexingManager(const std::string& database_path, const std::vector<std::string>& list_of_paths);

    void IndexFiles();

private:
    std::vector<char> barrels_distribution_ = {'a', 'g', 'n', 't'};
    std::vector<std::pair<std::filesystem::path, int>> files_;
    std::string database_path_;
    std::string barrel_path_pattern_;

    void FillBarrelFiles();

    void FillFilesInfo();

    void CreateFinalBarrelFiles();

    void WriteIntoAdditionalFile(auto& file, auto pos);

    void storeVarintNumber(int number, auto& file);

    std::vector<std::string> ParseLine(const std::string& line);

    int GetBarrelFile(char first_char);

    void GetListOfFiles(const std::vector<std::string>& list_of_paths);
};
