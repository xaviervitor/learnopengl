#pragma once

class FileString {
public:
    FileString(const char* path);
    ~FileString();

    char** getStringAddress();
private:
    char* str;
};