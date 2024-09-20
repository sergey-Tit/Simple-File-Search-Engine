#include "IndexingProgram.h"

IndexingManager::IndexingManager(const std::string& database_path, const std::vector<std::string>& list_of_paths) {
    database_path_ = database_path;
    barrel_path_pattern_ = database_path_ + "\\barrel_";
    for (int i = 0; i < 4; ++i) {
        auto new_barrel = barrel_path_pattern_ + std::to_string(i) + ".txt";
        std::ofstream file(new_barrel);
        file.close();
    }
    GetListOfFiles(list_of_paths);
}

void IndexingManager::IndexFiles() {
    FillBarrelFiles();
    FillFilesInfo();
    CreateFinalBarrelFiles();
}

void IndexingManager::FillBarrelFiles() {
    std::ofstream barrel_file0(barrel_path_pattern_ + "0.txt", std::ios::binary);
    std::ofstream barrel_file1(barrel_path_pattern_ + "1.txt", std::ios::binary);
    std::ofstream barrel_file2(barrel_path_pattern_ + "2.txt", std::ios::binary);
    std::ofstream barrel_file3(barrel_path_pattern_ + "3.txt", std::ios::binary);
    for (int i = 0; i < files_.size(); ++i) {
        auto& el = files_[i];
        std::string line;
        std::ifstream file(el.first, std::ios::binary);
        int cur_line = 0;
        while (std::getline(file, line)) {
            auto words = ParseLine(line);
            el.second += words.size();
            for (auto& el_ : words) {
                int index = GetBarrelFile(el_[0]);
                switch (index) {
                    case 0:
                        barrel_file0 << i << ' ' << el_ << ' ' << cur_line << '\n';
                        break;
                    case 1:
                        barrel_file1 << i << ' ' << el_ << ' ' << cur_line << '\n';
                        break;
                    case 2:
                        barrel_file2 << i << ' ' << el_ << ' ' << cur_line << '\n';
                        break;
                    case 3:
                        barrel_file3 << i << ' ' << el_ << ' ' << cur_line << '\n';
                        break;
                }
            }
            ++cur_line;
        }
        file.close();
    }
    barrel_file0.close();
    barrel_file1.close();
    barrel_file2.close();
    barrel_file3.close();
}

void IndexingManager::FillFilesInfo() {
    std::ofstream file(database_path_ + "\\files_info.txt", std::ios::binary);
    for (auto& el : files_) {
        file << el.second << ' ' << el.first << '\n';
    }
    file.close();
}

void IndexingManager::CreateFinalBarrelFiles() {
    for (int i = 0; i < 4; ++i) {
        std::string barrel_path = barrel_path_pattern_ + std::to_string(i) + ".txt";
        std::ifstream barrel_file(barrel_path, std::ios::binary);
        std::map<std::pair<std::string, int>, std::vector<int>> dict;
        std::string term;
        int did;
        int line_number;
        while (barrel_file >> did >> term >> line_number) {
            dict[std::make_pair(term, did)].push_back(line_number);
        }
        barrel_file.close();

        std::ofstream rewrite_file(barrel_path, std::ios::binary);
        std::ofstream additional_file(barrel_path_pattern_ + std::to_string(i) + "_additional" + ".txt", std::ios::binary);
        std::string prev_term = " ";
        for (auto& el : dict) {
            std::string cur_term = el.first.first;
            if (cur_term != prev_term) {
                auto pos = rewrite_file.tellp();
                WriteIntoAdditionalFile(additional_file, pos);
            }
            rewrite_file << el.first.first << ' ';
            storeVarintNumber(el.first.second, rewrite_file);
            storeVarintNumber(el.second.size(), rewrite_file);
            for (int j = 0; j < el.second.size(); ++j) {
                if (j == 0) {
                    storeVarintNumber(el.second[j], rewrite_file);
                }
                else {
                    storeVarintNumber(el.second[j] - el.second[j - 1], rewrite_file);
                }
            }
            prev_term = cur_term;
        }
        rewrite_file.close();
        additional_file.close();
    }
}

void IndexingManager::WriteIntoAdditionalFile(auto& file, auto pos) {
    auto pos_ = static_cast<size_t>(pos);
    std::vector<unsigned char> st;
    for (int j = 0; j < 8; ++j) {
        st.push_back(pos_ % (1 << 8));
        pos_ >>= 8;
    }
    std::reverse(st.begin(), st.end());
    for (auto& el : st) {
        file << el;
    }
}

void IndexingManager::storeVarintNumber(int number, auto& file) {
    std::vector<unsigned char> v;
    while (number != 0) {
        char b = number % (1 << 7);
        number >>= 7;
        v.push_back(b);
    }
    if (v.empty()) {
        v.push_back(0);
    }
    v[0] += (1 << 7);
    std::reverse(v.begin(), v.end());
    for (auto& el : v) {
        file << el;
    }
}

std::vector<std::string> IndexingManager::ParseLine(const std::string& line) {
    std::vector<std::string> res;
    std::string word;
    for (auto& el : line) {
        if (!ispunct(el) && el != '\t' && el != ' ') {
            word.push_back(tolower(el));
        }
        else {
            if (!word.empty()) {
                res.push_back(word);
            }
            word.clear();
        }
    }

    return res;
}

int IndexingManager::GetBarrelFile(char first_char) {
    int index = 0;
    for (int i = 0; i < 4; ++i) {
        if (first_char >= barrels_distribution_[i]) {
            index = i;
        }
    }

    return index;
}

void IndexingManager::GetListOfFiles(const std::vector<std::string>& list_of_paths) {
    for (auto& el : list_of_paths) {
        if (!std::filesystem::is_directory(el)) {
            files_.push_back(std::make_pair(el, 0));
            continue;
        }
        for (auto& el2 : std::filesystem::recursive_directory_iterator(el)) {
            if (!std::filesystem::is_directory(el2)) {
                files_.push_back(std::make_pair(el2, 0));
            }
        }
    }
}
