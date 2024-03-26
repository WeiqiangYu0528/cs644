#pragma once
#include <string>
//package joosc.ir.interpret;

/**
 * Constants for interpreting intermediate code.
 */
class Configuration {
public:
    /* Some special stack-related names that are used in the IR */
    /** Prefix for argument registers */
    static const std::string ABSTRACT_ARG_PREFIX;// = "_ARG";
    static const std::string ABSTRACT_RET;// = "_RET";

    /** Word size; assumes a 32-bit architecture */
    static const int WORD_SIZE = 4;
};

const std::string Configuration::ABSTRACT_ARG_PREFIX = "_ARG";
const std::string Configuration::ABSTRACT_RET = "_RET";

