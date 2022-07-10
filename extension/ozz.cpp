#ifdef EMSCRIPTEN

#include <emscripten.h>
#define HL_PRIM
#define HL_NAME(n)	EMSCRIPTEN_KEEPALIVE eb_##n
#define DEFINE_PRIM(ret, name, args)
#define _OPT(t) t*
#define _GET_OPT(value,t) *value
#define alloc_ref(r, _) r
#define alloc_ref_const(r,_) r
#define _ref(t)			t
#define _unref(v)		v
#define free_ref(v) delete (v)
#define HL_CONST const

#else

#define HL_NAME(x) ozz_##x
#include <hl.h>

// Need to link in helpers
HL_API hl_type hltx_ui16;
HL_API hl_type hltx_ui8;

#define _IDL _BYTES
#define _OPT(t) vdynamic *
#define _GET_OPT(value,t) (value)->v.t
template <typename T> struct pref {
	void (*finalize)( pref<T> * );
	T *value;
};

#define _ref(t) pref<t>
#define _unref(v) v->value
#define _unref_ptr_safe(v) (v != nullptr ? v->value : nullptr)
#define alloc_ref(r,t) _alloc_ref(r,finalize_##t)
#define alloc_ref_const(r, _) _alloc_const(r)
#define HL_CONST

template<typename T> void free_ref( pref<T> *r ) {
	if( !r->finalize ) hl_error("delete() is not allowed on const value.");
	delete r->value;
	r->value = NULL;
	r->finalize = NULL;
}

template<typename T> void free_ref( pref<T> *r, void (*deleteFunc)(T*) ) {
	if( !r->finalize ) hl_error("delete() is not allowed on const value.");
	deleteFunc( r->value );
	r->value = NULL;
	r->finalize = NULL;
}

// Float vector
struct _hl_float2 {
	float x;
	float y;
};

struct _hl_float3 {
	float x;
	float y;
	float z;
};

struct _hl_float4 {
	float x;
	float y;
	float z;
	float w;
};

// int vector
struct _hl_int2 {
	int x;
	int y;
};

struct _hl_int3 {
	int x;
	int y;
	int z;
};

struct _hl_int4 {
	int x;
	int y;
	int z;
	int w;
};

// double vector
struct _hl_double2 {
	double x;
	double y;
};

struct _hl_double3 {
	double x;
	double y;
	double z;
};

struct _hl_double4 {
	double x;
	double y;
	double z;
	double w;
};

inline void testvector(_hl_float3 *v) {
  printf("v: %f %f %f\n", v->x, v->y, v->z);
}
template<typename T> pref<T> *_alloc_ref( T *value, void (*finalize)( pref<T> * ) ) {
	if (value == nullptr) return nullptr;
	pref<T> *r = (pref<T>*)hl_gc_alloc_finalizer(sizeof(pref<T>));
	r->finalize = finalize;
	r->value = value;
	return r;
}

template<typename T> pref<T> *_alloc_const( const T *value ) {
	if (value == nullptr) return nullptr;
	pref<T> *r = (pref<T>*)hl_gc_alloc_noptr(sizeof(pref<T>));
	r->finalize = NULL;
	r->value = (T*)value;
	return r;
}

inline static varray* _idc_alloc_array(float *src, int count) {
	if (src == nullptr) return nullptr;

	varray *a = NULL;
	float *p;
	a = hl_alloc_array(&hlt_f32, count);
	p = hl_aptr(a, float);

	for (int i = 0; i < count; i++) {
		p[i] = src[i];
	}
	return a;
}
inline static varray* _idc_alloc_array(unsigned char *src, int count) {
	if (src == nullptr) return nullptr;

	varray *a = NULL;
	float *p;
	a = hl_alloc_array(&hltx_ui8, count);
	p = hl_aptr(a, float);

	for (int i = 0; i < count; i++) {
		p[i] = src[i];
	}
	return a;
}

inline static varray* _idc_alloc_array( char *src, int count) {
	return _idc_alloc_array((unsigned char *)src, count);
}

inline static varray* _idc_alloc_array(int *src, int count) {
	if (src == nullptr) return nullptr;

	varray *a = NULL;
	int *p;
	a = hl_alloc_array(&hlt_i32, count);
	p = hl_aptr(a, int);

	for (int i = 0; i < count; i++) {
		p[i] = src[i];
	}
	return a;

}

inline static varray* _idc_alloc_array(double *src, int count) {
	if (src == nullptr) return nullptr;

	varray *a = NULL;
	double *p;
	a = hl_alloc_array(&hlt_f64, count);
	p = hl_aptr(a, double);

	for (int i = 0; i < count; i++) {
		p[i] = src[i];
	}
	return a;
}


inline static varray* _idc_alloc_array(const unsigned short *src, int count) {
	if (src == nullptr) return nullptr;

	varray *a = NULL;
	unsigned short *p;
	a = hl_alloc_array(&hltx_ui16, count);
	p = hl_aptr(a, unsigned short);

	for (int i = 0; i < count; i++) {
		p[i] = src[i];
	}
	return a;
}

inline static varray* _idc_alloc_array(unsigned short *src, int count) {
	if (src == nullptr) return nullptr;

	varray *a = NULL;
	unsigned short *p;
	a = hl_alloc_array(&hltx_ui16, count);
	p = hl_aptr(a, unsigned short);

	for (int i = 0; i < count; i++) {
		p[i] = src[i];
	}
	return a;
}

inline static void _idc_copy_array( float *dst, varray *src, int count) {
	float *p = hl_aptr(src, float);
	for (int i = 0; i < count; i++) {
		dst[i] = p[i];
	}
}

inline static void _idc_copy_array( varray *dst, float *src,  int count) {
	float *p = hl_aptr(dst, float);
	for (int i = 0; i < count; i++) {
		p[i] = src[i];
	}
}


inline static void _idc_copy_array( int *dst, varray *src, int count) {
	int *p = hl_aptr(src, int);
	for (int i = 0; i < count; i++) {
		dst[i] = p[i];
	}
}

inline static void _idc_copy_array( unsigned short *dst, varray *src) {
	unsigned short *p = hl_aptr(src, unsigned short);
	for (int i = 0; i < src->size; i++) {
		dst[i] = p[i];
	}
}

inline static void _idc_copy_array( const unsigned short *cdst, varray *src) {
	unsigned short *p = hl_aptr(src, unsigned short);
	unsigned short *dst = (unsigned short *)cdst;
	for (int i = 0; i < src->size; i++) {
		dst[i] = p[i];
	}
}

inline static void _idc_copy_array( varray *dst, int *src,  int count) {
	int *p = hl_aptr(dst, int);
	for (int i = 0; i < count; i++) {
		p[i] = src[i];
	}
}


inline static void _idc_copy_array( double *dst, varray *src, int count) {
	double *p = hl_aptr(src, double);
	for (int i = 0; i < count; i++) {
		dst[i] = p[i];
	}
}

inline static void _idc_copy_array( varray *dst, double *src,  int count) {
	double *p = hl_aptr(dst, double);
	for (int i = 0; i < count; i++) {
		p[i] = src[i];
	}
}

#endif

#ifdef _WIN32
#pragma warning(disable:4305)
#pragma warning(disable:4244)
#pragma warning(disable:4316)
#endif

#include "ozz/base/platform.h"
#include "ozz/base/span.h"
#include "ozz/animation/runtime/animation.h"
#include "ozz/animation/runtime/local_to_model_job.h"
#include "ozz/animation/runtime/sampling_job.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/animation/runtime/track.h"
#include "ozz/animation/runtime/track_sampling_job.h"
#include "ozz/animation/runtime/track_triggering_job.h"
#include "ozz/base/log.h"
#include "ozz/base/maths/box.h"
#include "ozz/base/maths/simd_math.h"
#include "ozz/base/maths/soa_transform.h"
#include "ozz/base/maths/vec_float.h"
#include "ozz/options/options.h"
#include "ozz/base/containers/string.h"
#include "ozz/base/memory/unique_ptr.h"
#include "ozz/base/containers/vector.h"


#include "utils.h"
#include "mesh.h"
#include "skeleton.h"
#include "animation.h"
#include "model.h"





extern "C" {

static void finalize_SamplingJob( _ref(ozz::animation::SamplingJob)* _this ) { free_ref(_this ); }
HL_PRIM void HL_NAME(SamplingJob_delete)( _ref(ozz::animation::SamplingJob)* _this ) {
	free_ref(_this );
}
DEFINE_PRIM(_VOID, SamplingJob_delete, _IDL);
static void finalize_Part( _ref(Part)* _this ) { free_ref(_this ); }
HL_PRIM void HL_NAME(Part_delete)( _ref(Part)* _this ) {
	free_ref(_this );
}
DEFINE_PRIM(_VOID, Part_delete, _IDL);
static void finalize_Mesh( _ref(Mesh)* _this ) { free_ref(_this ); }
HL_PRIM void HL_NAME(Mesh_delete)( _ref(Mesh)* _this ) {
	free_ref(_this );
}
DEFINE_PRIM(_VOID, Mesh_delete, _IDL);
static void finalize_Skeleton( _ref(ozz::animation::Skeleton)* _this ) { free_ref(_this ); }
HL_PRIM void HL_NAME(Skeleton_delete)( _ref(ozz::animation::Skeleton)* _this ) {
	free_ref(_this );
}
DEFINE_PRIM(_VOID, Skeleton_delete, _IDL);
static void finalize_Animation( _ref(ozz::animation::Animation)* _this ) { free_ref(_this ); }
HL_PRIM void HL_NAME(Animation_delete)( _ref(ozz::animation::Animation)* _this ) {
	free_ref(_this );
}
DEFINE_PRIM(_VOID, Animation_delete, _IDL);
static void finalize_Model( _ref(Model)* _this ) { free_ref(_this ); }
HL_PRIM void HL_NAME(Model_delete)( _ref(Model)* _this ) {
	free_ref(_this );
}
DEFINE_PRIM(_VOID, Model_delete, _IDL);
HL_PRIM _ref(ozz::animation::SamplingJob)* HL_NAME(SamplingJob_new0)() {
	return alloc_ref((new ozz::animation::SamplingJob()),SamplingJob);
}
DEFINE_PRIM(_IDL, SamplingJob_new0,);

HL_PRIM bool HL_NAME(SamplingJob_validate0)(_ref(ozz::animation::SamplingJob)* _this) {
	return (_unref(_this)->Validate());
}
DEFINE_PRIM(_BOOL, SamplingJob_validate0, _IDL);

HL_PRIM float HL_NAME(SamplingJob_get_ratio)( _ref(ozz::animation::SamplingJob)* _this ) {
	return _unref(_this)->ratio;
}
DEFINE_PRIM(_F32,SamplingJob_get_ratio,_IDL);
HL_PRIM float HL_NAME(SamplingJob_set_ratio)( _ref(ozz::animation::SamplingJob)* _this, float value ) {
	_unref(_this)->ratio = (value);
	return value;
}
DEFINE_PRIM(_F32,SamplingJob_set_ratio,_IDL _F32);

HL_PRIM void HL_NAME(SamplingJob_setAnimation1)(_ref(ozz::animation::SamplingJob)* _this, _ref(ozz::animation::Animation)* anim) {
	(samplingjob_set_animation( _unref(_this) , _unref_ptr_safe(anim)));
}
DEFINE_PRIM(_VOID, SamplingJob_setAnimation1, _IDL _IDL);

HL_PRIM _ref(Part)* HL_NAME(Part_new0)() {
	return alloc_ref((new Part()),Part);
}
DEFINE_PRIM(_IDL, Part_new0,);

HL_PRIM float HL_NAME(Part_get_positions)( _ref(Part)* _this, int index ) {
	return _unref(_this)->positions[index];
}
DEFINE_PRIM(_F32,Part_get_positions,_IDL _I32);
HL_PRIM float HL_NAME(Part_set_positions)( _ref(Part)* _this, int index, float value ) {
	_unref(_this)->positions[index] = (value);
	return value;
}
DEFINE_PRIM(_F32,Part_set_positions,_IDL _I32 _F32); // Array setter

HL_PRIM int HL_NAME(Part_positions_count0)(_ref(Part)* _this) {
	return (_unref(_this)->positions_count());
}
DEFINE_PRIM(_I32, Part_positions_count0, _IDL);

HL_PRIM int HL_NAME(Part_vertex_count0)(_ref(Part)* _this) {
	return (_unref(_this)->vertex_count());
}
DEFINE_PRIM(_I32, Part_vertex_count0, _IDL);

HL_PRIM int HL_NAME(Part_influencesCount0)(_ref(Part)* _this) {
	return (_unref(_this)->influences_count());
}
DEFINE_PRIM(_I32, Part_influencesCount0, _IDL);

HL_PRIM _ref(Mesh)* HL_NAME(Mesh_new0)() {
	return alloc_ref((new Mesh()),Mesh);
}
DEFINE_PRIM(_IDL, Mesh_new0,);

HL_PRIM int HL_NAME(Mesh_triangle_index_count0)(_ref(Mesh)* _this) {
	return (_unref(_this)->triangle_index_count());
}
DEFINE_PRIM(_I32, Mesh_triangle_index_count0, _IDL);

HL_PRIM int HL_NAME(Mesh_vertex_count0)(_ref(Mesh)* _this) {
	return (_unref(_this)->vertex_count());
}
DEFINE_PRIM(_I32, Mesh_vertex_count0, _IDL);

HL_PRIM int HL_NAME(Mesh_max_influences_count0)(_ref(Mesh)* _this) {
	return (_unref(_this)->max_influences_count());
}
DEFINE_PRIM(_I32, Mesh_max_influences_count0, _IDL);

HL_PRIM bool HL_NAME(Mesh_skinned0)(_ref(Mesh)* _this) {
	return (_unref(_this)->skinned());
}
DEFINE_PRIM(_BOOL, Mesh_skinned0, _IDL);

HL_PRIM int HL_NAME(Mesh_num_joints0)(_ref(Mesh)* _this) {
	return (_unref(_this)->num_joints());
}
DEFINE_PRIM(_I32, Mesh_num_joints0, _IDL);

HL_PRIM unsigned short HL_NAME(Mesh_get_triangle_indices)( _ref(Mesh)* _this, int index ) {
	return _unref(_this)->triangle_indices[index];
}
DEFINE_PRIM(_I16,Mesh_get_triangle_indices,_IDL _I32);
HL_PRIM unsigned short HL_NAME(Mesh_set_triangle_indices)( _ref(Mesh)* _this, int index, unsigned short value ) {
	_unref(_this)->triangle_indices[index] = (value);
	return value;
}
DEFINE_PRIM(_I16,Mesh_set_triangle_indices,_IDL _I32 _I16); // Array setter

HL_PRIM unsigned short HL_NAME(Mesh_get_JointRemap)( _ref(Mesh)* _this, int index ) {
	return _unref(_this)->joint_remaps[index];
}
DEFINE_PRIM(_I16,Mesh_get_JointRemap,_IDL _I32);
HL_PRIM unsigned short HL_NAME(Mesh_set_JointRemap)( _ref(Mesh)* _this, int index, unsigned short value ) {
	_unref(_this)->joint_remaps[index] = (value);
	return value;
}
DEFINE_PRIM(_I16,Mesh_set_JointRemap,_IDL _I32 _I16); // Array setter

HL_PRIM int HL_NAME(Mesh_jointRemapCount0)(_ref(Mesh)* _this) {
	return (_unref(_this)->joint_remap_count());
}
DEFINE_PRIM(_I32, Mesh_jointRemapCount0, _IDL);

HL_PRIM int HL_NAME(Mesh_highestJointIndex0)(_ref(Mesh)* _this) {
	return (_unref(_this)->highest_joint_index());
}
DEFINE_PRIM(_I32, Mesh_highestJointIndex0, _IDL);

HL_PRIM void HL_NAME(Mesh_load2)(_ref(Mesh)* _this, vbyte* data, int length) {
	(_unref(_this)->load(data, length));
}
DEFINE_PRIM(_VOID, Mesh_load2, _IDL _BYTES _I32);

HL_PRIM int HL_NAME(Mesh_parts_count0)(_ref(Mesh)* _this) {
	return (_unref(_this)->parts_count());
}
DEFINE_PRIM(_I32, Mesh_parts_count0, _IDL);

HL_PRIM varray* HL_NAME(Mesh_get_buffer0)(_ref(Mesh)* _this) {
	return (_unref(_this)->get_buffer());
}
DEFINE_PRIM(_ARR, Mesh_get_buffer0, _IDL);

HL_PRIM varray* HL_NAME(Mesh_get_indices0)(_ref(Mesh)* _this) {
	return (_unref(_this)->get_indices());
}
DEFINE_PRIM(_ARR, Mesh_get_indices0, _IDL);

HL_PRIM vbyte* HL_NAME(Mesh_getRawBuffer0)(_ref(Mesh)* _this) {
	return (_unref(_this)->getRawBuffer());
}
DEFINE_PRIM(_BYTES, Mesh_getRawBuffer0, _IDL);

HL_PRIM _ref(ozz::animation::Skeleton)* HL_NAME(Skeleton_new0)() {
	return alloc_ref((new ozz::animation::Skeleton()),Skeleton);
}
DEFINE_PRIM(_IDL, Skeleton_new0,);

HL_PRIM void HL_NAME(Skeleton_load2)(_ref(ozz::animation::Skeleton)* _this, vbyte* data, int length) {
	(load_skeleton( _unref(_this) , data, length));
}
DEFINE_PRIM(_VOID, Skeleton_load2, _IDL _BYTES _I32);

HL_PRIM int HL_NAME(Skeleton_jointCount0)(_ref(ozz::animation::Skeleton)* _this) {
	return (_unref(_this)->num_joints());
}
DEFINE_PRIM(_I32, Skeleton_jointCount0, _IDL);

HL_PRIM int HL_NAME(Skeleton_soaJointCount0)(_ref(ozz::animation::Skeleton)* _this) {
	return (_unref(_this)->num_soa_joints());
}
DEFINE_PRIM(_I32, Skeleton_soaJointCount0, _IDL);

HL_PRIM int HL_NAME(Skeleton_jointNameCount0)(_ref(ozz::animation::Skeleton)* _this) {
	return (skeleton_num_joint_names( _unref(_this) ));
}
DEFINE_PRIM(_I32, Skeleton_jointNameCount0, _IDL);

HL_PRIM vbyte* HL_NAME(Skeleton_getJointName1)(_ref(ozz::animation::Skeleton)* _this, int idx) {
	return (skeleton_get_joint_name( _unref(_this) , idx));
}
DEFINE_PRIM(_BYTES, Skeleton_getJointName1, _IDL _I32);

HL_PRIM _ref(ozz::animation::Animation)* HL_NAME(Animation_new0)() {
	return alloc_ref((new ozz::animation::Animation()),Animation);
}
DEFINE_PRIM(_IDL, Animation_new0,);

HL_PRIM void HL_NAME(Animation_load2)(_ref(ozz::animation::Animation)* _this, vbyte* data, int length) {
	(load_animation( _unref(_this) , data, length));
}
DEFINE_PRIM(_VOID, Animation_load2, _IDL _BYTES _I32);

HL_PRIM int HL_NAME(Animation_trackCount0)(_ref(ozz::animation::Animation)* _this) {
	return (_unref(_this)->num_tracks());
}
DEFINE_PRIM(_I32, Animation_trackCount0, _IDL);

HL_PRIM int HL_NAME(Animation_soaTrackCount0)(_ref(ozz::animation::Animation)* _this) {
	return (_unref(_this)->num_soa_tracks());
}
DEFINE_PRIM(_I32, Animation_soaTrackCount0, _IDL);

HL_PRIM vbyte* HL_NAME(Animation_name0)(_ref(ozz::animation::Animation)* _this) {
	return const_char_to_vbytes(_unref(_this)->name());
}
DEFINE_PRIM(_BYTES, Animation_name0, _IDL);

HL_PRIM float HL_NAME(Animation_duration0)(_ref(ozz::animation::Animation)* _this) {
	return (_unref(_this)->duration());
}
DEFINE_PRIM(_F32, Animation_duration0, _IDL);

HL_PRIM _ref(Model)* HL_NAME(Model_new0)() {
	return alloc_ref((new Model()),Model);
}
DEFINE_PRIM(_IDL, Model_new0,);

HL_PRIM bool HL_NAME(Model_loadMeshes2)(_ref(Model)* _this, vbyte* data, int length) {
	return (_unref(_this)->loadMeshes(data, length));
}
DEFINE_PRIM(_BOOL, Model_loadMeshes2, _IDL _BYTES _I32);

HL_PRIM bool HL_NAME(Model_loadSkeleton2)(_ref(Model)* _this, vbyte* data, int length) {
	return (_unref(_this)->loadSkeleton(data, length));
}
DEFINE_PRIM(_BOOL, Model_loadSkeleton2, _IDL _BYTES _I32);

HL_PRIM bool HL_NAME(Model_runSamplingJob1)(_ref(Model)* _this, _ref(ozz::animation::SamplingJob)* job) {
	return (_unref(_this)->runSamplingJob(_unref_ptr_safe(job)));
}
DEFINE_PRIM(_BOOL, Model_runSamplingJob1, _IDL _IDL);

HL_PRIM varray* HL_NAME(Model_getSkinMatrices1)(_ref(Model)* _this, int meshIndex) {
	return (_unref(_this)->getSkinMatrices(meshIndex));
}
DEFINE_PRIM(_ARR, Model_getSkinMatrices1, _IDL _I32);

HL_PRIM varray* HL_NAME(Model_getMeshes0)(_ref(Model)* _this) {
	return (_unref(_this)->getMeshes());
}
DEFINE_PRIM(_ARR, Model_getMeshes0, _IDL);

HL_PRIM HL_CONST _ref(ozz::animation::Skeleton)* HL_NAME(Model_getSkeleton0)(_ref(Model)* _this) {
	return alloc_ref_const((_unref(_this)->getSkeleton()),Skeleton);
}
DEFINE_PRIM(_IDL, Model_getSkeleton0, _IDL);

}
