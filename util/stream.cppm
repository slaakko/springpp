// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

export module util.stream;

import std.core;

export namespace util {

enum class Origin
{
    seekSet, seekCur, seekEnd
};

class StreamObserver;

class Stream
{
public:
    Stream();
    virtual ~Stream();
    virtual int ReadByte() = 0;
    virtual int64_t Read(uint8_t* buf, int64_t count) = 0;
    virtual void Write(uint8_t x) = 0;
    virtual void Write(uint8_t* buf, int64_t count) = 0;
    virtual void Flush();
    virtual void Seek(int64_t pos, Origin origin);
    virtual int64_t Tell();
    void CopyTo(Stream& destination);
    void CopyTo(Stream& destination, int64_t bufferSize);
    int64_t Position() const { return position; }
    void SetPosition(int64_t position_);
    void AddObserver(StreamObserver* observer);
    void RemoveObserver(StreamObserver* observer);
private:
    int64_t position;
    std::vector<StreamObserver*> observers;
};

class StreamObserver
{
public:
    StreamObserver();
    virtual ~StreamObserver();
    virtual void PositionChanged(Stream* stream) {}
};

class Streams
{
public:
    Streams();
    Streams(Streams&& that) noexcept;
    Streams(const Streams&) = delete;
    Streams& operator=(Streams&& that) noexcept;
    Streams& operator=(const Streams&) = delete;
    ~Streams();
    void Add(Stream* stream);
    Stream* Get(int index) const;
    Stream& Back() const;
    int Count() const { return static_cast<int>(streams.size()); }
private:
    std::vector<std::unique_ptr<Stream>> streams;
};

} // util
