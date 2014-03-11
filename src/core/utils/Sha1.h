#ifndef _SHA1_H_
#define _SHA1_H_

#include <co/Platform.h>

namespace sha1 {

using co::uint8;
using co::uint32;

union Block64
{
	uint8 c[64];
	uint32 l[16];
};

struct Context
{
	uint32 state[5];
	uint32 count[2];
	Block64 buffer;
};

const uint8 DIGEST_SIZE = 20;

void init( Context* context );
void update( Context* context, const uint8* data, const uint32 length );
void final( Context* context, uint8 digest[DIGEST_SIZE] );

} // namespace sha1

#endif
