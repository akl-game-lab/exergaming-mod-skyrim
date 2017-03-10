#include "ITypes.h"

Bitstring::Bitstring()
:content(NULL)
{
	
}

Bitstring::Bitstring(UInt32 inLength)
:content(NULL)
{
	Alloc(inLength);
}

Bitstring::~Bitstring()
{
	Dispose();
}

void Bitstring::Alloc(UInt32 inLength)
{
	Dispose();

	inLength = (inLength + 7) & ~7;
	length = inLength >> 3;

	content = new UInt8[length];
}

void Bitstring::Dispose(void)
{
	delete [] content;
}

void Bitstring::Clear(void)
{
	std::memset(content, 0, length);
}

void Bitstring::Clear(UInt32 idx)
{
	ASSERT_STR(idx < (length << 3), "Bitstring::Clear: out of range");

	content[idx >> 3] &= ~(1 << (idx & 7));
}

void Bitstring::Set(UInt32 idx)
{
	ASSERT_STR(idx < (length << 3), "Bitstring::Set: out of range");

	content[idx >> 3] |= (1 << (idx & 7));
}

bool Bitstring::IsSet(UInt32 idx)
{
	ASSERT_STR(idx < (length << 3), "Bitstring::IsSet: out of range");

	return (content[idx >> 3] & (1 << (idx & 7))) ? true : false;
}

bool Bitstring::IsClear(UInt32 idx)
{
	ASSERT_STR(idx < (length << 3), "Bitstring::IsClear: out of range");

	return (content[idx >> 3] & (1 << (idx & 7))) ? false : true;
}
