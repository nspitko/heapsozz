#include "utils.h"

#include "ozz/base/maths/simd_math.h"

#ifndef EMSCRIPTEN
varray *span_of_const_char_const_to_varray( ozz::span<const char* const> input )
{
	varray* varr = hl_alloc_array( &hlt_bytes, (int)input.size() );
	vbyte** buff = hl_aptr(varr, vbyte*);


	for( size_t i=0; i<input.size();i++)
	{
		buff[i] = const_char_to_vbytes( input[i] );
	}

	return varr;
}

vbyte* const_char_to_vbytes(const char* str)
{
	int size = int(strlen(str) + 1);

	#ifdef EMSCRIPTEN
	vbyte* result = (vbyte*) malloc( sizeof(char) * ( size + 1 ) );
	#else
	vbyte* result = hl_alloc_bytes(size);
	#endif
	memcpy(result, str, size);
	return result;
}
#endif
// -------------------------------------------------------------------------------------------------
// SamplingJob
// -------------------------------------------------------------------------------------------------
void samplingjob_set_animation( ozz::animation::SamplingJob *job, ozz::animation::Animation *anim)
{
	job->animation = anim;
}