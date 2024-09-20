#include "IndexingProgramParser.h"

void IndexingExitWithError(const std::string& error_text) {
    std::cerr << error_text << '\n';
    std::cerr << "Follow this structure:\n";
    std::cerr << "--create <database_directory_path> --files <path_1> <path_2> ... <path_n>\n";
    std::cerr << "--create <database_directory_path> - creates a directory for database of indexing files\n";
    std::cerr << "--files <path_1> <path_2> ... <path_n> - paths of files to index\n";
    exit(EXIT_FAILURE);
}

IndexingOptions IndexingParse(int32_t argc, char** argv) {
    IndexingOptions options;
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    if (args[0] != "--create") {
        IndexingExitWithError("First argument is not --create");
    }
    if (args[2] != "--files") {
        IndexingExitWithError("Third argument is not --files");
    }
    if (!std::filesystem::create_directory(args[1])) {
        IndexingExitWithError("Incorrect database directory path. The path must indicate a non-existent folder.");
    }

    options.directory_name = args[1];
    for (int32_t i = 3; i < args.size(); ++i) {
        if (!std::filesystem::exists(args[i])) {
            IndexingExitWithError("Incorrect file path: " + args[i]);
        }
        options.files.push_back(args[i]);
    }

    return options;
}
