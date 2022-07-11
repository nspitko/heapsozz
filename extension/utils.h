
#include "ozz/base/containers/vector.h"
#include "ozz/base/span.h"
#include "ozz/animation/offline/raw_skeleton.h"
#include "ozz/animation/runtime/animation.h"
#include "ozz/animation/runtime/local_to_model_job.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/animation/runtime/track.h"
#include "ozz/animation/runtime/sampling_job.h"


#define EMSCRIPTEN 1
// hl will stomp emscripten so re-stomp them if needed
#ifdef EMSCRIPTEN


#include <emscripten.h>
#include <emscripten/bind.h>

typedef char vbyte;

#else
#include <hl.h>


	#ifndef _ref
		#define _ref(t) pref<t>
		#define _unref(v) v->value
		#define _unref_ptr_safe(v) (v != nullptr ? v->value : nullptr)
		#define alloc_ref(r,t) _alloc_ref(r,finalize_##t)
		#define alloc_ref_const(r, _) _alloc_const(r)
	#endif


	template<typename T> void free_ref( pref<T> *r );
	template<typename T> void free_ref( pref<T> *r, void (*deleteFunc)(T*) );

	template<typename T> pref<T> *_alloc_ref( T *value, void (*finalize)( pref<T> * ) );
	template<typename T> pref<T> *_alloc_const( const T *value );

	// WebIDL refs
	template <typename T> struct pref;


	// Converters
	varray *span_of_const_char_const_to_varray( ozz::span<const char* const> input );
	vbyte* const_char_to_vbytes(const char* str);

	void samplingjob_set_animation( ozz::animation::SamplingJob *job, ozz::animation::Animation *anim);


#endif



struct Mesh;

