#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <iterator>
#include <iostream>

struct IndexingOptions {
    std::vector<std::string> files;
    std::string directory_name;
};

void IndexingExitWithError(const std::string& error_text);

IndexingOptions IndexingParse(int32_t argc, char** argv);
