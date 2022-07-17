#include "skeleton.h"
#include "utils.h"

#include "ozz/base/io/archive.h"
#include "ozz/base/io/stream.h"

#ifdef EMSCRIPTEN
static bool load_skeleton(ozz::animation::Skeleton &skeleton, std::string data, int len)
#else
bool load_skeleton(ozz::animation::Skeleton *skeleton, vbyte* data, int len)
#endif
{
	printf("Loading skeleton archive\n" );


	ozz::io::MemoryStream ms;
	#ifdef EMSCRIPTEN
	ms.Write( data.data(), len );
	#else
	ms.Write( data, len );
	#endif
	ms.Seek(0,ms.kSet);

	ozz::io::IArchive archive(&ms);

	if (!archive.TestTag<ozz::animation::Skeleton>())
	{
		printf("Failed to load skeleton from file\n");
		return false;
	}

	// Once the tag is validated, reading cannot fail.
	#ifdef EMSCRIPTEN
	archive >> skeleton;
	#else
	archive >> *skeleton;
	#endif

	return true;
}

int skeleton_num_joint_names( ozz::animation::Skeleton *skeleton )
{
	return static_cast<int>( skeleton->joint_names().size() );
}
#ifndef EMSCRIPTEN
vbyte *skeleton_get_joint_name(ozz::animation::Skeleton *skeleton, int idx)
{
	return const_char_to_vbytes( skeleton->joint_names()[idx] );
}
#endif


#ifdef EMSCRIPTEN

using namespace emscripten;


EMSCRIPTEN_BINDINGS(ozzSkeleton) {
	class_<ozz::animation::Skeleton>("Skeleton")
		.constructor<>()
		.function("loadImpl", &load_skeleton)

		.property("numJoints", &ozz::animation::Skeleton::num_joints)
		.property("numSoaJoints", &ozz::animation::Skeleton::num_soa_joints)
		.property("jointParents", &ozz::animation::Skeleton::joint_parents)
		//.property("joint_names", &Mesh::joint_names)

		//.function("load", &Mesh::load)
		//.class_property("name", &animation_get_name)
    ;
}
#else

static void skeleton_finalize(hl_skeleton *s)
{
	printf("Skeleton was deallocated!!\n");
	s->skeleton.~Skeleton();
}

HL_PRIM hl_skeleton* HL_NAME(skeleton_init)()
{
	hl_skeleton* hl_mem = (hl_skeleton*)hl_gc_alloc_finalizer(sizeof(hl_skeleton));
	hl_mem->finalize = skeleton_finalize;
	new (&hl_mem->skeleton)ozz::animation::Skeleton();
	return hl_mem;
}

HL_PRIM bool HL_NAME(skeleton_load)(hl_skeleton* s, vbyte* data, int length) {
	return load_skeleton(&s->skeleton, data, length);
}

HL_PRIM int HL_NAME(skeleton_num_joints)(hl_skeleton* s) {
	return s->skeleton.num_joints();
}

HL_PRIM int HL_NAME(skeleton_num_soa_joints)(hl_skeleton* s) {
	return s->skeleton.num_soa_joints();
}

DEFINE_PRIM(_STRUCT, skeleton_init, _NO_ARG);
DEFINE_PRIM(_BOOL, skeleton_load, _STRUCT _BYTES _I32);
DEFINE_PRIM(_I32, skeleton_num_joints, _STRUCT);
DEFINE_PRIM(_I32, skeleton_num_soa_joints, _STRUCT);



#endif