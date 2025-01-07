#pragma once

#include "../spimp/exceptions.hpp"
#include "elpdef.hpp"

#include <fstream>

class ElpReader {
  private:
    uint32_t index = 0;
    std::ifstream file;
    string path;

    MetaInfo readMetaInfo();

    ObjInfo readObjInfo();

    ClassInfo readClassInfo();

    FieldInfo readFieldInfo();

    TypeParamInfo readTypeParamInfo();

    MethodInfo readMethodInfo();

    MethodInfo::LineInfo readLineInfo();

    MethodInfo::ExceptionTableInfo readExceptionInfo();

    MethodInfo::LocalInfo readLocalInfo();

    MethodInfo::ArgInfo readArgInfo();

    MethodInfo::MatchInfo readMatchInfo();

    MethodInfo::MatchInfo::CaseInfo readCaseInfo();

    GlobalInfo readGlobalInfo();

    CpInfo readCpInfo();

    __Container readContainer();

    __UTF8 readUTF8();

    uint8_t readByte() {
        index++;
        return static_cast<uint8_t>(file.get());
    }

    uint16_t readShort() {
        uint16_t a = readByte();
        uint8_t b = readByte();
        return a << 8 | b;
    }

    uint32_t readInt() {
        uint32_t a = readShort();
        uint16_t b = readShort();
        return a << 16 | b;
    }

    uint64_t readLong() {
        uint64_t a = readInt();
        uint32_t b = readInt();
        return a << 32 | b;
    }

    [[noreturn]] void corruptFileError() const { throw err::CorruptFileError(path); }

  public:
    explicit ElpReader(const string &path) : file(path, std::ios::in | std::ios::binary) {
        if (!file.is_open()) throw err::FileNotFoundError(path);
    }

    ElpReader(const ElpReader &other) = delete;
    ElpReader(ElpReader &&other) noexcept = default;
    ElpReader &operator=(const ElpReader &other) = delete;
    ElpReader &operator=(ElpReader &&other) noexcept = default;
    ~ElpReader() = default;

    /**
     * This function parses the file associated with this reader
     * and returns the bytecode data
     * @return The bytecode data in the form of ElpInfo
     */
    ElpInfo read();

    const string &getPath() const { return path; }
};
