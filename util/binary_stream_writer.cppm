// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

export module util.binary.stream.writer;
import std.core;
import util.stream;
import util.uuid;

export namespace util {

class BinaryStreamWriter
{
public:
    BinaryStreamWriter(Stream& stream_);
    Stream& GetStream() { return stream; }
    void Write(bool x);
    void Write(uint8_t x);
    void Write(int8_t x);
    void Write(uint16_t x);
    void Write(int16_t x);
    void Write(uint32_t x);
    void Write(int32_t x);
    void Write(uint64_t x);
    void Write(int64_t x);
    void Write(float x);
    void Write(double x);
    void Write(char x);
    void Write(char8_t x);
    void Write(char16_t x);
    void Write(char32_t x);
    void Write(const std::string& s);
    void Write(const std::string& s, bool writeNull);
    void Write(const std::u16string& s);
    void Write(const std::u32string& s);
    void WriteULEB128UInt(uint32_t x);
    void WriteULEB128ULong(uint64_t x);
    void WriteSLEB128Int(int32_t x);
    void WriteSLEB128Long(int64_t x);
    void Write(const uuid& uuid);
    void WriteTime(time_t time);
    int64_t Position() const { return stream.Position(); }
private:
    Stream& stream;
};

} // util
