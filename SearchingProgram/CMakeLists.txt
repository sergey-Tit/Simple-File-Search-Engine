add_executable(searching
        main.cpp
        SearchingProgramParser.cpp "SearchingProgramParser.h"
        SearchingProgram.cpp "SearchingProgram.h")

add_library(Searching
        SearchingProgramParser.cpp "SearchingProgramParser.h"
        SearchingProgram.cpp "SearchingProgram.h")