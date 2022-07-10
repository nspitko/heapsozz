#include "utils.h"

#include "ozz/base/maths/simd_math.h"



varray *span_of_const_char_const_to_varray( ozz::span<const char* const> input )
{
	varray* varr = hl_alloc_array( &hlt_bytes, input.size() );
	vbyte** buff = hl_aptr(varr, vbyte*);


	for( int i=0; i<input.size();i++)
	{
		buff[i] = const_char_to_vbytes( input[i] );
	}

	return varr;
}

vbyte* const_char_to_vbytes(const char* str)
{
	int size = int(strlen(str) + 1);
	vbyte* result = hl_alloc_bytes(size);
	memcpy(result, str, size);
	return result;
}

// -------------------------------------------------------------------------------------------------
// Float4x4
// -------------------------------------------------------------------------------------------------

// @todo this sucks. Ideally we shouldn't have to alloc a new varray to pass this and just
// access the members directly somehow; but 16 getters over the dll boundary is probably
// worse than the varray alloc + copy here.
varray *float4x4_get( ozz::math::Float4x4 *mat )
{
	varray *arr = hl_alloc_array(&hlt_f32, 4*4);
	void *farr = hl_aptr(arr,void);

	memcpy(farr, (void*)mat->cols, sizeof(float) * 4 * 4);

	return arr;

}

// -------------------------------------------------------------------------------------------------
// SamplingJob
// -------------------------------------------------------------------------------------------------
void samplingjob_set_animation( ozz::animation::SamplingJob *job, ozz::animation::Animation *anim)
{
	job->animation = anim;
}