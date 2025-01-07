#pragma once

#include "reader.hpp"

class ElpWriter {
  private:
    string path;
    FILE *file;

    void write(uint8_t i) { fputc(i, file); }

    void write(uint16_t i) {
        write(static_cast<uint8_t>(i >> 8));
        write(static_cast<uint8_t>(i & 0xFF));
    }

    void write(uint32_t i) {
        write(static_cast<uint16_t>(i >> 16));
        write(static_cast<uint16_t>(i & 0xFFFF));
    }

    void write(uint64_t i) {
        write(static_cast<uint32_t>(i >> 32));
        write(static_cast<uint32_t>(i & 0xFFFFFFFF));
    }

    void write(CpInfo info);

    void write(__UTF8 utf);

    void write(__Container con);

    void write(GlobalInfo info);

    void write(ObjInfo info);

    void write(MethodInfo info);

    void write(MethodInfo::LineInfo line);

    void write(MethodInfo::ArgInfo info);

    void write(MethodInfo::LocalInfo info);

    void write(MethodInfo::ExceptionTableInfo info);

    void write(MethodInfo::MatchInfo info);

    void write(MethodInfo::MatchInfo::CaseInfo info);

    void write(ClassInfo info);

    void write(FieldInfo info);

    void write(TypeParamInfo info);

    void write(MetaInfo info);

  public:
    explicit ElpWriter(const string &filename);

    /**
     * Writes the binary information given in the form of ElpInfo
     * in binary form which is readable by ElpReader to the file specified
     * during constructing the object
     * @param elp ELP information object
     */
    void write(ElpInfo elp);

    /**
     * Closes the file
     */
    void close() const;

    const string &getPath() const { return path; }

    FILE *getFile() const { return file; }
};