#include "SearchingProgramParser.h"
#include "SearchingProgram.h"

int main(int argc, char** argv) {
    auto options_ = SearchingParse(argc, argv);
    SearchingManager searching_manager(options_.database_path, options_.k_coef, options_.b_coef, options_.number_of_files_to_show);
    searching_manager.completeSearching(options_.query);
}
