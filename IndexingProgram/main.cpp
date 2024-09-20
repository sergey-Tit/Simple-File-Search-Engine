#include "IndexingProgramParser.h"
#include "IndexingProgram.h"

int main(int argc, char** argv) {
    auto options = IndexingParse(argc, argv);
    IndexingManager indexing_manager(options.directory_name, options.files);
    indexing_manager.IndexFiles();
}
