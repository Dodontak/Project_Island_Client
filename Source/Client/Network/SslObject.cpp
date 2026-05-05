#include "SslObject.h"

// OpenSSL의 UI 구조체 이름을 잠시 바꿔서 언리얼의 namespace UI와 충돌을 피함
#define UI UI_ST
#include <openssl/ssl.h>
#include <openssl/err.h>
#undef UI
// --- OpenSSL 충돌 방지 처리 끝 ---

SslObject::~SslObject()
{
	SSL_free(_ssl);
}

void SslObject::Init(SSL_CTX* ctx)
{
	check(_ssl = SSL_new(ctx));
	check(_rbio = BIO_new(BIO_s_mem()));
	check(_wbio = BIO_new(BIO_s_mem()));
	SSL_set_bio(_ssl, _rbio, _wbio);
}

// rbio에 적힌 데이터를 읽어서 핸드쉐이크 하는 함수. (클라 -> 서버)
// 보낼 데이터가 있으면 wbio에 적히고, 데이터는 꺼내서 직접 보내야함.
SslStatus SslObject::Connect()
{
	FScopeLock Lock(&Mutex);
	int32 ret = SSL_connect(_ssl);

	if (ret == 1)
		return SslStatus::Ok;

	int32 err = SSL_get_error(_ssl, ret);
	if (err == SSL_ERROR_WANT_READ)
		return SslStatus::WantRead;
	else if (err == SSL_ERROR_WANT_WRITE)
		return SslStatus::WantWrite;
	else
		return SslStatus::Fail;
}

// rbio에 적힌 데이터를 복호화 하는 함수
SslStatus SslObject::Read(BYTE* buffer, size_t readSize, size_t* readLen)
{
	FScopeLock Lock(&Mutex);
	int32 ret = SSL_read_ex(_ssl, buffer, readSize, readLen);
	if (ret == 0) // 실패
	{
		int32 err = SSL_get_error(_ssl, ret);
		if (err == SSL_ERROR_WANT_READ) //복호화 하기에 데이터 부족함
			return SslStatus::WantRead;
		else if (err == SSL_ERROR_ZERO_RETURN)
			return SslStatus::Shutdown;
		else
			return SslStatus::Fail;
	}
	return SslStatus::Ok;
}

// wbio에 데이터를 암호화 해서 쓰는 함수
SslStatus SslObject::Write(BYTE* buffer, size_t dataLen, size_t* writtenLen)
{
	FScopeLock Lock(&Mutex);
	int32 ret = SSL_write_ex(_ssl, buffer, dataLen, writtenLen);
	if (ret == 0) // 실패 사실상 발생하지 않음.
		return SslStatus::Fail;
	return SslStatus::Ok;
}

uint32 SslObject::GetRBioPendingSize()
{
	return BIO_pending(_rbio);
}

uint32 SslObject::GetWBioPendingSize()
{
	return BIO_pending(_wbio);
}

// bio에 암/복호화 없이 직접 읽거나 쓰는 함수들.
// 리턴값 > 0 읽거나 쓴 바이트 수, 0 -1 실패, -2 BIO 오류
uint32 SslObject::ReadRBio(BYTE* buffer, int32 readSize)
{
	return BIO_read(_rbio, buffer, readSize);
}

uint32 SslObject::WriteRBio(BYTE* buffer, int32 writeSize)
{
	return BIO_write(_rbio, buffer, writeSize);
}

uint32 SslObject::ReadWBio(BYTE* buffer, int32 readSize)
{
	return BIO_read(_wbio, buffer, readSize);
}

uint32 SslObject::WriteWBio(BYTE* buffer, int32 dataLen)
{
	return BIO_write(_wbio, buffer, dataLen);
}
