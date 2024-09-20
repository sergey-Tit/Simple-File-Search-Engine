#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <string>
#include <iostream>
#include <map>
#include <cmath>
#include <set>
#include <stack>
#include <algorithm>

class SearchingManager {
public:
    SearchingManager(const std::string& database_path, int k_coef, int b_coef, int number_of_files_to_show);

    void completeSearching(const std::vector<std::string>& query);

    std::vector<std::string> getTopFiles(const std::vector<std::string>& query);

private:
    std::vector<char> barrels_distribution_ = {'a', 'g', 'n', 't'};
    std::vector<std::pair<std::string, int>> files_;
    std::string database_path_;
    std::string barrel_path_pattern_;
    std::map<std::string, std::map<int, int>> posting_lists_;
    std::set<std::pair<double, int>, std::greater<>> top_heap_;
    std::vector<std::string> terms_in_query_;
    double average_length_;
    int number_of_files_;
    int k_coef_;
    int b_coef_;
    int number_of_files_to_show_;

    void printTopFiles();

    void getPostingList(const std::string& term);

    double getRelevance(const std::string& term, int did);

    std::vector<std::string> parseQuery(const std::vector<std::string>& query);

    std::vector<int> getStartDocuments(const std::vector<std::string>& query);

    void calculateRelevancyOfDocuments(const std::vector<int>& documents);

    size_t getTermPosition(const std::string& term, int barrel_file_index);

    int GetBarrelFile(char first_char);

    int getVarintNumber(auto& file);
};
