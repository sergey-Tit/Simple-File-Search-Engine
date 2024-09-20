add_executable(indexing
        main.cpp
        IndexingProgramParser.cpp "IndexingProgramParser.h"
        IndexingProgram.cpp "IndexingProgram.h")

add_library(Indexing
        IndexingProgramParser.cpp "IndexingProgramParser.h"
        IndexingProgram.cpp "IndexingProgram.h")