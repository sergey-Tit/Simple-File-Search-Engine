#include "SearchingProgram.h"

SearchingManager::SearchingManager(const std::string& database_path, int k_coef, int b_coef, int number_of_files_to_show) :
        database_path_(database_path), k_coef_(k_coef), b_coef_(b_coef), number_of_files_to_show_(number_of_files_to_show) {
    barrel_path_pattern_ = database_path_ + "\\barrel_";
    std::ifstream files_info(database_path_ + "\\files_info.txt", std::ios::binary);
    std::string file_name;
    long long length;
    long long summary_length = 0;
    while (files_info >> length >> file_name) {
        files_.push_back(std::make_pair(file_name.substr(1, file_name.size() - 2), length));
        summary_length += length;
    }
    average_length_ = static_cast<double>(summary_length) / files_.size();
    number_of_files_ = files_.size();
    files_info.close();
};

void SearchingManager::completeSearching(const std::vector<std::string>& query) {
    auto sp = parseQuery(query);
    auto list_of_documents = getStartDocuments(sp);
    calculateRelevancyOfDocuments(list_of_documents);
    printTopFiles();
}

std::vector<std::string> SearchingManager::getTopFiles(const std::vector<std::string>& query) {
    auto sp = parseQuery(query);
    auto list_of_documents = getStartDocuments(sp);
    calculateRelevancyOfDocuments(list_of_documents);
    std::vector<std::string> result;
    if (top_heap_.empty()) {
        result.push_back("No relevant files to your query!");
        return result;
    }
    int count = 0;
    for (auto& el : top_heap_) {
        result.push_back(files_[el.second].first);
        ++count;
        if (count == number_of_files_to_show_) {
            break;
        }
    }

    return result;
}

void SearchingManager::printTopFiles() {
    int count = 0;
    if (top_heap_.empty()) {
        std::cout << "No relevant files to your query!";
    }
    for (auto& el : top_heap_) {
        int did = el.second;
        std::cout << files_[el.second].first << '\n';
        for (auto& term : posting_lists_) {
            std::string term_ = term.first;
            std::cout << term_ << " - " << term.second[did] << " times: ";
            if (term.second[did] != 0) {
                int barrel_file_index = GetBarrelFile(term_[0]);
                size_t pos = getTermPosition(term_, barrel_file_index);
                std::ifstream file(barrel_path_pattern_ + std::to_string(barrel_file_index) + ".txt", std::ios::binary);
                file.seekg(pos);
                std::string st;
                char byte;
                file >> st;
                file >> std::noskipws >> byte;
                int did_ = getVarintNumber(file);
                while (did_ != did) {
                    int cnt = getVarintNumber(file);
                    for (int i = 0; i < cnt; ++i) {
                        getVarintNumber(file);
                    }
                    file >> st;
                    file >> std::noskipws >> byte;
                    did_ = getVarintNumber(file);
                }
                int cnt = getVarintNumber(file);
                int num = 0;
                for (int i = 0; i < cnt; ++i) {
                    num += getVarintNumber(file);
                    std::cout << num << ' ';
                }
                file.close();
            }
            std::cout << '\n';
        }
        ++count;
        if (count == number_of_files_to_show_) {
            break;
        }
        std::cout << '\n';
    }
}

void SearchingManager::getPostingList(const std::string& term) {
    int barrel_file_index = GetBarrelFile(term[0]);
    std::map<int, int> res;
    size_t pos = getTermPosition(term, barrel_file_index);
    std::ifstream file(barrel_path_pattern_ + std::to_string(barrel_file_index) + ".txt", std::ios::binary);
    file.seekg(pos);
    std::string st;
    char byte;
    file >> st;
    file >> std::noskipws >> byte;
    while (st == term) {
        int did = getVarintNumber(file);
        int count = getVarintNumber(file);
        res[did] = count;
        for (int i = 0; i < count; ++i) {
            getVarintNumber(file);
        }
        file >> st;
        file >> std::noskipws >> byte;
    }
    file.close();

    posting_lists_[term] = res;
}

double SearchingManager::getRelevance(const std::string& term, int did) {
    double num_1 = posting_lists_[term][did] * (k_coef_ + 1);
    double num_2 = posting_lists_[term][did] + k_coef_ * (1 - b_coef_ + b_coef_ *
            (static_cast<double>(files_[did].second)) / average_length_);
    double num_3 = num_1 / num_2;
    double num_4 = static_cast<double>(number_of_files_) - posting_lists_[term].size() + 0.5;
    double num_5 = static_cast<double>(posting_lists_[term].size()) + 0.5;
    double num_6 = num_4 / num_5;
    double num_7 = std::log2(num_6);

    return num_3 * num_7;
}

std::vector<std::string> SearchingManager::parseQuery(const std::vector<std::string>& query) {
    std::stack<std::string> operators;
    std::vector<std::string> output;
    std::map<std::string, int> priorities = {{"OR", 0}, {"AND", 1}};
    for (auto& el : query) {
        if (el[0] == '(') {
            std::string term;
            for (int i = 0; i < el.size(); ++i) {
                if (el[i] == '(') {
                    operators.push("(");
                }
                else {
                    term += el[i];
                }
            }
            output.push_back(term);
        }
        else if (el.back() == ')') {
            std::string term;
            bool flag = true;
            for (int i = 0; i < el.size(); ++i) {
                if (el[i] == ')') {
                    if (flag) {
                        output.push_back(term);
                        flag = false;
                    }
                    while (operators.top() != "(") {
                        output.push_back(operators.top());
                        operators.pop();
                    }
                    operators.pop();
                }
                else {
                    term += el[i];
                }
            }
        }
        else if (el == "AND" || el == "OR") {
            if (operators.empty() || operators.top() == "(" || priorities[el] > priorities[operators.top()]) {
                operators.push(el);
            }
            else {
                while (!operators.empty() || operators.top() != "OR") {
                    output.push_back(operators.top());
                    operators.pop();
                }
                output.push_back(el);
            }
        }
        else {
            output.push_back(el);
        }
    }
    while (!operators.empty()) {
        output.push_back(operators.top());
        operators.pop();
    }

    return output;
}

std::vector<int> SearchingManager::getStartDocuments(const std::vector<std::string>& query) {
    std::stack<std::vector<int>> stack;
    std::vector<std::vector<int>> cur_operands;
    for (auto& el : query) {
        if (el == "OR") {
            std::vector<int> first, second, res;
            if (cur_operands.size() == 0) {
                first = stack.top();
                stack.pop();
                second = stack.top();
                stack.pop();
            }
            else if (cur_operands.size() == 1) {
                first = stack.top();
                stack.pop();
                second = cur_operands[0];
                cur_operands.clear();
            }
            else {
                first = cur_operands[0];
                second = cur_operands[1];
                cur_operands.clear();
            }
            std::merge(first.begin(), first.end(), second.begin(), second.end(), std::back_inserter(res));
            std::set<int> mn(res.begin(), res.end());
            std::vector<int> res1(mn.begin(), mn.end());
            stack.push(res1);
        }
        else if (el == "AND") {
            std::vector<int> first, second, res;
            if (cur_operands.size() == 0) {
                first = stack.top();
                stack.pop();
                second = stack.top();
                stack.pop();
            }
            else if (cur_operands.size() == 1) {
                first = stack.top();
                stack.pop();
                second = cur_operands[0];
                cur_operands.clear();
            }
            else {
                first = cur_operands[0];
                second = cur_operands[1];
                cur_operands.clear();
            }
            std::set_intersection(first.begin(), first.end(), second.begin(), second.end(), std::back_inserter(res));
            std::set<int> mn(res.begin(), res.end());
            std::vector<int> res1(mn.begin(), mn.end());
            stack.push(res1);
        }
        else {
            getPostingList(el);
            terms_in_query_.push_back(el);
            std::vector<int> posting_list;
            for (auto& el2 : posting_lists_[el]) {
                posting_list.push_back(el2.first);
            }
            cur_operands.push_back(posting_list);
        }
    }
    if (stack.empty()) {
        return cur_operands[0];
    }

    return stack.top();
}

void SearchingManager::calculateRelevancyOfDocuments(const std::vector<int>& documents) {
    double relevancy = 0;
    for (auto& el : documents) {
        for (auto &el2: terms_in_query_) {
            relevancy += getRelevance(el2, el);
        }
        top_heap_.insert(std::make_pair(relevancy, el));
    }
}

size_t SearchingManager::getTermPosition(const std::string& term, int barrel_file_index) {
    std::ifstream file(barrel_path_pattern_ + std::to_string(barrel_file_index) + ".txt",std::ios::binary);
    std::ifstream additional_file(barrel_path_pattern_ + std::to_string(barrel_file_index) + "_additional.txt",
                       std::ios_base::ate | std::ios::binary);
    size_t file_size = additional_file.tellg();
    additional_file.seekg(0, std::ios::beg);
    size_t L = 0, R = (file_size / 8) + 1;
    size_t ans = 0;
    while (R - L > 1) {
        size_t mid = (R + L) / 2;
        size_t pos = mid * 8;
        size_t pos_ = 0;
        unsigned char a;
        additional_file.seekg(pos);
        for (int i = 0; i < 8; i++) {
            additional_file >> std::noskipws >> a;
            pos_ <<= 8;
            pos_ += a;
        }
        file.seekg(pos_);
        std::string term_;
        file >> term_;
        if (term < term_) {
            R = mid;
        }
        else {
            L = mid;
            ans = pos_;
        }
    }
    file.close();
    additional_file.close();

    return ans;
}

int SearchingManager::GetBarrelFile(char first_char) {
    int index = 0;
    for (int i = 0; i < 4; ++i) {
        if (first_char >= barrels_distribution_[i]) {
            index = i;
        }
    }

    return index;
}

int SearchingManager::getVarintNumber(auto& file) {
    int number = 0;
    unsigned char byte;
    do {
        file >> std::noskipws >> byte;
        number <<= 7;
        number += byte;
    } while (!(byte >> 7));
    number -= 1 << 7;

    return number;
}
