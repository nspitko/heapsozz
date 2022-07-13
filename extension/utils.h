#pragma once

#include "ozz/base/containers/vector.h"
#include "ozz/base/span.h"
#include "ozz/animation/offline/raw_skeleton.h"
#include "ozz/animation/runtime/animation.h"
#include "ozz/animation/runtime/local_to_model_job.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/animation/runtime/track.h"
#include "ozz/animation/runtime/sampling_job.h"


//#define EMSCRIPTEN 1
#ifdef EMSCRIPTEN


#include <emscripten.h>
#include <emscripten/bind.h>

typedef char vbyte;

#endif
#ifdef HL

	#define HL_NAME(n) ozz_##n

	#include <hl.h>


	#define _TMODEL _ABSTRACT(ozzmodel)
	#define _TSKELETON _ABSTRACT(ozzskeleton)
	#define _TMESH _ABSTRACT(ozzmesh)

	// Converters
	varray *span_of_const_char_const_to_varray( ozz::span<const char* const> input );
	vbyte* const_char_to_vbytes(const char* str);

#endif


