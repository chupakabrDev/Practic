#include "replace.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "finder.h"
#include "reader.h"
#include "util.h"
#include "writer.h"

int replace(const char* inputPath, const char* outputPath, const char* patHex, const char* repHex, const size_t blockSize) {
    size_t patternLen, replacementLen;
    unsigned char* pattern = hexToBytes(patHex, &patternLen);
    if (!pattern) {
        perror("Неверный hex паттерна");
        return EXIT_FAILURE;
    }

    unsigned char* replacement = hexToBytes(repHex, &replacementLen);
    if (!replacement) {
        free(pattern);
        perror("Неверный hex замены");
        return EXIT_FAILURE;
    }

    if (patternLen == 0) {
        perror("Паттерн не может быть пустым");
        free(pattern);
        free(replacement);
        return EXIT_FAILURE;
    }

    Reader* reader = createReader(inputPath, blockSize);
    if (!reader) {
        perror("Ошибка создания Reader");
        free(pattern);
        free(replacement);
        return EXIT_FAILURE;
    }

    Writer* writer = createWriter(outputPath);
    if (!writer) {
        perror("Ошибка создания Writer");
        destroyReader(reader);
        free(pattern);
        free(replacement);
        return EXIT_FAILURE;
    }

    Finder* finder = createFinder(pattern, patternLen);
    if (!finder) {
        perror("Ошибка создания Finder");
        destroyWriter(writer);
        destroyReader(reader);
        free(pattern);
        free(replacement);
        return EXIT_FAILURE;
    }


    size_t bytesRead;
    size_t matchedD = 0;
    while ((bytesRead = readNext(reader)) > 0) {
        for (size_t i = 0; i < bytesRead; i++) {
            unsigned char c = reader->buffer[i];

            switch (find(finder, &c, 1)) {
                case FIND_SUCCESS: {
                    writeNext(writer, (const char*)replacement, replacementLen);
                    matchedD = 0;
                    break;
                }
                case FIND_FAILURE: {
                    if (finder->matched <= matchedD) {
                        if (matchedD == 0)
                            writeNext(writer, (const char*)&c, 1);
                        else {
                            if (finder->matched == 0) {
                                writeNext(writer, (const char*)pattern, matchedD);

                                writeNext(writer, (const char*)&c, 1);
                            } else
                                writeNext(writer, (const char*)pattern, patternLen - finder->matched);

                            printf("%zu, %zu\n", patternLen, finder->matched);
                            matchedD = 0;
                        }
                    }
                    else
                        matchedD = finder->matched;

                    break;
                }
                case FIND_ERROR: {
                    perror("Ошибка поиска");
                    goto cleanup;
                }
            }

        }
    }

    // Успех
    destroyFinder(finder);
    destroyWriter(writer);
    destroyReader(reader);
    free(pattern);
    free(replacement);
    return EXIT_SUCCESS;

cleanup:
    destroyFinder(finder);
    destroyWriter(writer);
    destroyReader(reader);
    free(pattern);
    free(replacement);
    return EXIT_FAILURE;
}
