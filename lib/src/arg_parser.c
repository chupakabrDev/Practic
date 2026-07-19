#include "arg_parser.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util.h"

static constexpr char ADD_SEP_NAME[] = "d";
static constexpr char SET_SEP_NAME[] = "D";
static constexpr char HELP_NAME[] = "h";

static const OptionInfo addSep = {ADD_SEP_NAME, STR, "Добавить разделитель"};
static const OptionInfo setSep = {SET_SEP_NAME, STR, "Заменить текущие разделители"};
static const OptionInfo help = {HELP_NAME, NONE, "Показать справку"};

static const OptionInfo defaultOptions[] = {addSep, setSep, help};
static constexpr size_t defOptionsCount = sizeof(defaultOptions) / sizeof(OptionInfo);

Option * createOption(OptionInfo *optInfo, const OptValue val, char **error) {
    if (!optInfo)
        return putError("Неверные аргументы: createOption", error);

    Option *result = malloc(sizeof(Option));
    if (!result) return putError("Ошибка выделения памяти: createOption", error);

    result->info = optInfo;
    result->val = val;

    return result;
}

ArgParser *createArgParser(OptionInfo *options, const size_t optionsCount, const ArgsCompatibilityValidator validator) {
    if (!options || !optionsCount || !validator) return nullptr;

    ArgParser *argParser = malloc(sizeof(ArgParser));
    if (!argParser) return nullptr;

    argParser->options = options;
    argParser->optionsCount = optionsCount;
    argParser->validator = validator;

    argParser->separators = malloc(2);
    if (!argParser->separators) {
        free(argParser);
        return nullptr;
    }
    argParser->separators[0] = '=';
    argParser->separators[1] = ':';
    argParser->separatorsCount = 2;

    argParser->result = nullptr;
    argParser->resultSize = 0;

    return argParser;
}

Option* parseArg(const OptionInfo *options, const size_t optionsCount, char *separators, size_t separatorsCount, char *currentToken, char *nextToken, char **error) {
    for (size_t k = 0; k < optionsCount; k++) {
        OptionInfo optInfo = options[k];
        if (!startsWith(currentToken, optInfo.name)) continue;

        const size_t currentTokenLen = strlen(currentToken);
        const size_t argNameLen = strlen(optInfo.name);
        if (optInfo.type == NONE) {
            if (currentTokenLen != argNameLen)
                return nullptr;

            Option *opt = createOption(&optInfo, (OptValue){0}, error);
            if (error) return nullptr;

            return opt;
        }

        char *rawVal;
        if (currentTokenLen > argNameLen) {
            rawVal = currentToken + argNameLen;
            rawVal += startsWithAny(rawVal, separators, separatorsCount) ? 1 : 0;
        } else {
            if (!nextToken || startsWith(nextToken, "-"))
                return putErrorf("Аргумент %s не имеет значения\n", error, currentToken);

            rawVal = nextToken;
        }

        OptValue val = (OptValue){.strVal = rawVal};
        if (optInfo.type == INT) {
            if (!strIsDigit(rawVal))
                return putErrorf("Аргумент %s должен иметь целочисленный аргумент(сейчас %s)\n", error, currentToken, rawVal);

            val = (OptValue){.intVal = strtolSafe(rawVal, nullptr, 10)};
            if (errno == ERANGE) return putErrorf("Целочисленный аргумент %s переполнен(%s)\n", error, currentToken, rawVal);
        }

        Option *opt = createOption(&optInfo, val, error);
        if (error) return nullptr;

        return opt;
    }

    return nullptr;
}

void parseArgs(ArgParser *parser, const int argc, char *argv[]) {
    if (!parser || !parser->options || !parser->optionsCount || !argc || !argv) return;

    char *error = NULL;

    for (int i = 0; i < argc; i++) {
        if (argv[i][0] != '-') continue;

        char *currToken = argv[i] + 1;
        char *nextToken = i + 1 < argc ? argv[i + 1] : nullptr;

        const Option *standardOption = parseArg(defaultOptions, defOptionsCount, parser->separators, parser->separatorsCount, currToken, nextToken, &error);
        if (error) goto err;

        if (standardOption) {
            const char *name = standardOption->info->name;
            if (name == ADD_SEP_NAME) {
                char *newSeps = realloc(parser->separators, parser->separatorsCount + 1);
                if (!newSeps) {
                    perror("Ошибка выделения памяти: parseArgs");
                    return;
                }

                newSeps[parser->separatorsCount++] = standardOption->val.strVal[0];
                parser->separators = newSeps;
            } else if (name == SET_SEP_NAME) {
                char *newSeps = realloc(parser->separators, 1);
                if (!newSeps) {
                    perror("Ошибка выделения памяти: parseArgs");
                    return;
                }

                newSeps[0] = standardOption->val.strVal[0];
                parser->separators = newSeps;
                parser->separatorsCount = 1;
            } else if (name == HELP_NAME) {
                for (size_t j = 0; j < parser->optionsCount; j++) {
                    printf("-%s - %s\n", parser->options[j].name, parser->options[j].description);
                }

                free(parser->result);
                parser->result = nullptr;
                parser->resultSize = 0;
                
                return;
            } else {
                fprintf(stderr, "Неизвестноее имя стандартной опции %s", name);
                return;
            }
        }

        Option *option = parseArg(parser->options, parser->optionsCount, parser->separators, parser->separatorsCount, currToken, nextToken, &error);
        if (error) goto err;

        if (option) {
            addResult(parser, option, &error);
            if (error) goto err;
        }

    }

    if (!parser->validator(parser->result, parser->resultSize, &error))
        goto err;

    return;

    err: {
        fprintf(stderr, "%s\n", error);
        free(error);
    }
}

Option *nextOption(ArgParser *parser) {
    if (!parser || !parser->result || !parser->resultSize) return nullptr;

    return &parser->result[--parser->resultSize];
}

bool addResult(ArgParser *parser, Option *option, char **error) {
    if (!parser || !parser->options || !parser->optionsCount || !option) {
        putError("Неверные аргументы", error);
        return false;
    }

    const size_t newSize = parser->resultSize + 1;
    Option *newResult = realloc(parser->result, newSize * sizeof(Option));
    if (!newResult) {
        putError("Ошибка выделения памяти: addResult", error);
        return false;
    }

    parser->result = newResult;
    parser->result[parser->resultSize++] = *option;

    return true;
}

void destroyArgParser(ArgParser *parser) {
    if (!parser) return;

    if (parser->result != nullptr)
        free(parser->result);

    free(parser->separators);

    free(parser);
}
