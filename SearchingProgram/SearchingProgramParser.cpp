#include <cstdint>
#include "SearchingProgramParser.h"

void SearchingExitWithError(const std::string& error_text) {
    std::cerr << error_text << '\n';
    std::cerr << "Follow this structure:\n";
    std::cerr << "--database <database_directory_path> --k_coef <k_coef> --b_coef <b_coef> ";
    std::cerr << "--top_files <number> --search_query <search_query>\n";
    std::cerr << "--database <database_directory_path> - correct path to the database directory with indexed files\n";
    std::cerr << "--k_coef <k_coef> - positive integer number, how much do repetitive terms affect relevance\n";
    std::cerr << "--b_coef <b_coef> - positive integer number, how much does the length of a document affect relevance\n";
    std::cerr << "--top_files <number> - positive integer number, number of top relevant documents to show\n";
    std::cerr << "--search_query <search_query> - the query to be searched for\n";
    exit(EXIT_FAILURE);
}

bool IsDigit(const std::string& number) {
    for (auto& el : number) {
        if (!isdigit(el)) {
            return false;
        }
    }

    return true;
}

bool CheckCorrectBracketSequence(const std::vector<std::string>& sp) {
    int cnt = 0;
    for (auto& el : sp) {
        for (auto& el2 : el) {
            if (el2 == '(') {
                ++cnt;
            }
            if (el2 == ')') {
                --cnt;
            }
        }
    }

    if (cnt == 0) {
        return true;
    }
    return false;
}

bool CheckRotation(const std::vector<std::string>& sp) {
    for (int i = 0; i < sp.size(); ++i) {
        if ((i % 2) == 0 && (sp[i] == "AND" || sp[i] == "OR")) {
            return false;
        }
        if ((i % 2) == 1 && sp[i] != "AND" && sp[i] != "OR") {
            return false;
        }
    }

    return true;
}

SearchingOptions SearchingParse(int argc, char** argv) {
    SearchingOptions options;
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    if (args[0] != "--database") {
        SearchingExitWithError("First argument is not --database");
    }
    if (args[2] != "--k_coef") {
        SearchingExitWithError("Second argument is not --k_coef");
    }
    if (args[4] != "--b_coef") {
        SearchingExitWithError("Third argument is not --b_coef");
    }
    if (args[6] != "--top_files") {
        SearchingExitWithError("Fourth argument is not --top_files");
    }
    if (args[8] != "--search_query") {
        SearchingExitWithError("Fifth argument is not --search_query");
    }

    if (!IsDigit(args[3])) {
        SearchingExitWithError("Incorrect k_coef. Must be positive integer number");
    }
    options.k_coef = std::stoi(args[3]);
    if (!IsDigit(args[5])) {
        SearchingExitWithError("Incorrect b_coef. Must be positive integer number");
    }
    options.b_coef = std::stoi(args[5]);
    if (!IsDigit(args[7])) {
        SearchingExitWithError("Incorrect number of top files. Must be positive integer number");
    }
    options.number_of_files_to_show = std::stoi(args[7]);

    if (!std::filesystem::exists(args[1])) {
        SearchingExitWithError("Incorrect database directory path");
    }
    for (int i = 0; i < 4; ++i) {
        if (!std::filesystem::exists(args[1] + "\\barrel_" + std::to_string(i) + ".txt")) {
            SearchingExitWithError("Incorrect database directory path");
        }
        if (!std::filesystem::exists(args[1] + "\\barrel_" + std::to_string(i) + "_additional.txt")) {
            SearchingExitWithError("Incorrect database directory path");
        }
    }
    if (!std::filesystem::exists(args[1] + "\\files_info.txt")) {
        SearchingExitWithError("Incorrect database directory path");
    }
    options.database_path = args[1];

    for (int i = 9; i < args.size(); ++i) {
        std::string st = args[i];
        while (st[0] == '(' && st.back() == ')') {
            st = st.substr(1, st.size() - 2);
        }
        std::vector<int> open_brackets, close_brackets;
        for (int j = 0; j < st.size(); ++j) {
            if (st[j] == '(') {
                open_brackets.push_back(j);
            }
            if (st[j] == ')') {
                close_brackets.push_back(j);
            }
        }
        if (!open_brackets.empty() && !close_brackets.empty()) {
            SearchingExitWithError("Incorrect query.");
        }
        if (!open_brackets.empty()) {
            for (int j = 0; j < open_brackets.size(); ++j) {
                if (open_brackets[j] != j) {
                    SearchingExitWithError("Incorrect query.");
                }
            }
        }
        else {
            std::reverse(close_brackets.begin(), close_brackets.end());
            for (int j = 0; j < close_brackets.size(); ++j) {
                if (close_brackets[j] != st.size() - j - 1) {
                    SearchingExitWithError("Incorrect query.");
                }
            }
        }
        options.query.push_back(st);
    }
    if (options.query.size() % 2 == 0 || !CheckRotation(options.query) || !CheckCorrectBracketSequence(options.query)) {
        SearchingExitWithError("Incorrect query.");
    }

    return options;
}
