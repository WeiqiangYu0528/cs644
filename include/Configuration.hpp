#pragma once

namespace TIR {

struct Configuration {
    static constexpr const char* ABSTRACT_ARG_PREFIX = "_ARG";
    static constexpr const char* ABSTRACT_RET = "_RET";
    static constexpr const char* STATIC_INIT_FUNC = "_STATIC_INIT";
    static constexpr int WORD_SIZE = 4;
};

};
