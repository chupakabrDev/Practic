#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "arg_parser.h"
#include "passgen.h"
#include "util.h"

typedef struct Args {
    int minl;
    int maxl;
    int len;

    int count;

    char *alphabet;
    char *set;
} Args;

static Args *args;

bool validateArgs(const Option *options, const size_t optionsCount, char **error) {

    for (size_t i = 0; i < optionsCount; i++) {
        const Option option = options[i];
        const char *name = option.info->name;

        if (strcmp(name, "minl") == 0) {
            if (args->minl != -1) {
                putErrorf("Аргумент %s уже был применен\n", error, name);
                return false;
            }
            args->minl = option.val.intVal;
        } else if (strcmp(name, "maxl") == 0) {
            if (args->maxl != -1) {
                putErrorf("Аргумент %s уже был применен\n", error, name);
                return false;
            }
            args->maxl = option.val.intVal;
        } else if (strcmp(name, "l") == 0) {
            if (args->len != -1) {
                putErrorf("Аргумент %s уже был применен\n", error, name);
                return false;
            }
            args->len = option.val.intVal;
        } else if (strcmp(name, "c") == 0) {
            if (args->count != -1) {
                putErrorf("Аргумент %s уже был применен\n", error, name);
                return false;
            }
            args->count = option.val.intVal;
        } else if (strcmp(name, "a") == 0) {
            if (args->alphabet != nullptr) {
                putErrorf("Аргумент %s уже был применен\n", error, name);
                return false;
            }
            args->alphabet = option.val.strVal;
        } else if (strcmp(name, "C") == 0) {
            if (args->set != nullptr) {
                putErrorf("Аргумент %s уже был применен\n", error, name);
                return false;
            }
            args->set = option.val.strVal;
        }
    }
    
    if (args->alphabet != nullptr && args->set != nullptr) {
        putError("Аргументы a и C несовместимы\n", error);
        return false;
    }

    const bool hasLen   = args->len != -1;
    const bool hasMinl  = args->minl != -1;
    const bool hasMaxl  = args->maxl != -1;

    if (hasLen) {
        if (hasMinl || hasMaxl) {
            putError("Аргумент l несовместим с minl/maxl\n", error);
            return false;
        }
    } else if (hasMinl || hasMaxl) {
        if (!hasMinl || !hasMaxl) {
            putError("Должны быть заданы оба аргумента minl и maxl\n", error);
            return false;
        }
        
        if (args->minl > args->maxl) {
            putError("minl не может быть больше maxl\n", error);
            return false;
        }
    } else args->len = 8;

    if (args->count == -1)
        args->count = 1;

    if (args->set == nullptr && args->alphabet == nullptr)
        args->set = "aADS";
    
    return true;
}

static OptionInfo optionInfos[] = {
    {"minl", INT, "Минимальная длина пароля"},
    {"maxl", INT, "Максимальная длина пароля"},
    {"l", INT, "Длина пароля"},
    {"c", INT, "Кол-во авролей"},
    {"a", STR, "Алфавит символов"},
    {"C", STR, "Набор символов (указывается один или несколько символов из множества {a, A, D, S}), a - маленькие латинские символы, A - большие латинские символы, D - цифры, S - спецсимволы"},
};

static bool initArgs() {
    args = malloc(sizeof(Args));
    if (!args) {
        perror("Ошибка выделения памяти: main");
        return false;
    }
    args->minl = -1;
    args->maxl = -1;
    args->len = -1;
    args->count = -1;
    args->alphabet = nullptr;
    args->set = nullptr;

    return true;
}

int main(const int argc, char *argv[]) {
    if (!initArgs()) {
        perror("Ошибка инициализации статичной переменной: Args");
        return 1;
    }


    srand(time(nullptr));

    ArgParser *argParser = createArgParser(optionInfos, 6, validateArgs);
    if (!argParser) {
        perror("Ошибка создания парсера: main");
        return 1;
    }

    if (!parseArgs(argParser, argc - 1, argv + 1)) {
        destroyArgParser(argParser);
        return 1;
    }

    char *alphabet = args->alphabet ? args->alphabet : buildAlphabetFromSet(args->set);
    if (!alphabet) {
        perror("Ошибка построения алфавита");
        return 1;
    }

    for (int i = 0; i < args->count; i++) {
        char *buffer = malloc(args->len * sizeof(char));
        if (!buffer) {
            perror("Ошибка выделения памяти: main");
            break;
        }

        generatePassword(alphabet, args->len, buffer);
        puts(buffer);
        free(buffer);
    }

    destroyArgParser(argParser);
    return 0;
}
