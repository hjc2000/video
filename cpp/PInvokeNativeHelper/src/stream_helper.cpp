#include "stream_helper.h"
#include<FileStream.h>
#include<iostream>

using namespace std;

StreamHandle *GetFileStream(std::string *url)
{
	shared_ptr<FileStream> fs = FileStream::Open(url->c_str());
	return new StreamHandle{ fs };
}

void FreeStream(StreamHandle *handle)
{
	delete handle;
}

uint8_t Stream_CanRead(StreamHandle *handle)
{
	return handle->_stream->CanRead();
}

uint8_t Stream_CanWrite(StreamHandle *handle)
{
	return handle->_stream->CanWrite();
}

uint8_t Stream_CanSeek(StreamHandle *handle)
{
	return handle->_stream->CanSeek();
}

int64_t Stream_Length(StreamHandle *handle)
{
	return handle->_stream->Length();
}

void Stream_SetLength(StreamHandle *handle, int64_t value)
{
	handle->_stream->SetLength(value);
}

int64_t Stream_Read(StreamHandle *handle, uint8_t *dst_buf, int64_t offset, int64_t count)
{
	return handle->_stream->Read(dst_buf, offset, count);
}

void Stream_Write(StreamHandle *handle, uint8_t *src_buf, int64_t offset, int64_t count)
{
	handle->_stream->Write(src_buf, offset, count);
}

void Stream_Flush(StreamHandle *handle)
{
	handle->_stream->Flush();
}

void Stream_Close(StreamHandle *handle)
{
	handle->_stream->Close();
}

int64_t Stream_Position(StreamHandle *handle)
{
	return handle->_stream->Position();
}

void Stream_SetPosition(StreamHandle *handle, int64_t value)
{
	handle->_stream->SetPosition(value);
}
