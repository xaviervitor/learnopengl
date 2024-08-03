#include "filestring.hpp"

#include <cstdio>
#include <cstdlib>

FileString::FileString(const char* path) {
    FILE* file = fopen(path, "rb");

    // get buffer size
    fseek(file, 0L, SEEK_END);
    size_t size = (size_t) ftell(file);
    rewind(file);

    // alloc and assign file to buffer
    str = (char*) malloc(size + 1);
    fread(str, size * sizeof(char), 1, file);

    // if malloc fails, handle it and return
    // if fread fails, handle it and return

    fclose(file);

    str[size] = '\0';
}

FileString::~FileString() {
    free(str);
}

char** FileString::getStringAddress() {
    return &str;
}