/*
Реализация writer.h

Кинев Алексей Александрович
МК-102
*/

#include "writer.h"

#include <stdlib.h>

#include "util.h"

Writer* createWriter(const char* pathToFile) {
    if (pathToFile == nullptr) return nullptr;

    Writer* writer = malloc(sizeof(Writer));
    if (writer == nullptr) return nullptr;

    FILE* file = fopen(pathToFile, "wb");
    if (file == nullptr) {
        free(writer);
        return nullptr;
    }

    writer->file = file;

    return writer;
}

size_t writeNext(const Writer *writer, const char *data, const size_t size) {
    if (writer == nullptr || writer->file == nullptr) return 0;

    return fwrite(data, sizeof(char), size, writer->file);
}

void destroyWriter(Writer *writer) {
    if (writer == nullptr) return;

    fclose(writer->file);
    free(writer);
}
