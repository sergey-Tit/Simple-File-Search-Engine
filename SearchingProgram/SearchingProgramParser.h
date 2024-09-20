#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm>

struct SearchingOptions {
    int k_coef;
    int b_coef;
    int number_of_files_to_show;
    std::string database_path;
    std::vector<std::string> query;
};

void SearchingExitWithError(const std::string& error_text);

bool IsDigit(const std::string& number);

bool CheckCorrectBracketSequence(const std::vector<std::string>& sp);

bool CheckRotation(const std::vector<std::string>& sp);

SearchingOptions SearchingParse(int argc, char** argv);
