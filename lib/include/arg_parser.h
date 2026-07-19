#pragma once
#include <stddef.h>

typedef enum OptValueType {
    NONE,
    INT,
    STR
} OptValueType;

typedef union {
    int intVal;
    char *strVal;
} OptValue;

typedef struct OptionInfo {
    const char *name; // имя опции (без '-')
    OptValueType type;
    const char *description;
} OptionInfo;

typedef struct {
    const OptionInfo *info;
    OptValue val;
} Option;

typedef bool (*ArgsCompatibilityValidator)(const Option *options, size_t optionsCount, char **error);

typedef struct ArgParser {
    OptionInfo *options;
    size_t optionsCount;
    Option *result;
    size_t resultSize;
    ArgsCompatibilityValidator validator;
    char *separators;
    size_t separatorsCount;
} ArgParser;

Option *createOption(const OptionInfo *optInfo, OptValue val, char **error);

ArgParser *createArgParser(OptionInfo *options, size_t optionsCount, ArgsCompatibilityValidator validator);

bool parseArgs(ArgParser *parser, int argc, char *argv[]);

Option *nextOption(ArgParser *parser);

bool addResult(ArgParser *parser, Option *option, char **error);

void destroyArgParser(ArgParser *parser);
