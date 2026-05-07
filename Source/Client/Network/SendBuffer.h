#pragma once

class SendBuffer : public TSharedFromThis<SendBuffer>
{
public:
	SendBuffer() = default;
	SendBuffer(BYTE* buffer, uint32 dataLen);
	SendBuffer(uint32 dataLen); //for AppendBuffer
	~SendBuffer();

public: //use at fill buffer
	bool AppendBuffer(BYTE* buffer, uint32 dataLen);
	BYTE* WritePos() { return &_buffer[_writePos]; }

public: //use at write
	BYTE* GetBuffer() { return &_buffer[_readPos]; }
	BYTE* GetPosPtr(uint32 pos) { return &_buffer[pos]; }
	uint32 GetFreeSize() { return _allocSize - _writePos; }
	uint32 GetDataLen() { return _writePos - _readPos; }
	bool OnWrite(uint32 dataLen);
	bool OnRead(uint32 dataLen);

private:
	uint32 _readPos = 0;
	uint32 _writePos = 0;
	uint32 _allocSize = 0;
	TArray<BYTE> _buffer;
};
