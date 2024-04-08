#pragma once

#include <string>
#include <unordered_set>
#include <vector>


namespace TIR {

struct Configuration {
    static constexpr const char* ABSTRACT_ARG_PREFIX = "_ARG";
    static constexpr const char* ABSTRACT_RET = "_RET";
    static constexpr const char* STATIC_INIT_FUNC = "_STATIC_INIT";
    static constexpr int WORD_SIZE = 4;

    static const std::unordered_set<std::string>& getLibraryFunctions() {
        static const std::unordered_set<std::string> libraryFunctions = {
            "__malloc", "__debexit", "__exception", "NATIVEjava.io.OutputStream.nativeWrite"
        };
        return libraryFunctions;
    }    
};

};
