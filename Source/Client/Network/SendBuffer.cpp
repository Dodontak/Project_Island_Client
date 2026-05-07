#include "SendBuffer.h"

SendBuffer::SendBuffer(BYTE* buffer, uint32 dataLen)
	: _writePos(dataLen), _allocSize(dataLen)
{
	_buffer.SetNum(dataLen);
	memcpy(&_buffer[0], buffer, dataLen);
}

SendBuffer::SendBuffer(uint32 dataLen) : _writePos(0), _allocSize(dataLen)
{
	_buffer.SetNum(dataLen);
}

SendBuffer::~SendBuffer() {}

bool	SendBuffer::AppendBuffer(BYTE* buffer, uint32 dataLen)
{
	if (_writePos + dataLen > _allocSize)
		return false;
	memcpy(&_buffer[_writePos], buffer, dataLen);
	_writePos += dataLen;
	return true;
}

bool SendBuffer::OnWrite(uint32 dataLen)
{
	if (dataLen > GetFreeSize())
		return false;
	_writePos += dataLen;
	return true;
}

bool SendBuffer::OnRead(uint32 dataLen)
{
	if (dataLen > GetDataLen())
		return false;
	_readPos += dataLen;
	return true;
}
