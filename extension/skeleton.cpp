#include "skeleton.h"
#include "utils.h"

#include "ozz/base/io/archive.h"
#include "ozz/base/io/stream.h"

bool load_skeleton(ozz::animation::Skeleton *skeleton, vbyte* data, int len)
{
	printf("Loading skeleton archive\n" );


	ozz::io::MemoryStream ms;
	ms.Write( data, len );
	ms.Seek(0,ms.kSet);

	ozz::io::IArchive archive(&ms);

	if (!archive.TestTag<ozz::animation::Skeleton>())
	{
		printf("Failed to load skeleton from file\n");
		return false;
	}

	// Once the tag is validated, reading cannot fail.
	archive >> *skeleton;

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

		.property("numJoints", &ozz::animation::Skeleton::num_joints)
		.property("numSoaJoints", &ozz::animation::Skeleton::num_soa_joints)
		.property("jointParents", &ozz::animation::Skeleton::joint_parents)
		//.property("joint_names", &Mesh::joint_names)

		//.function("load", &Mesh::load)
		//.class_property("name", &animation_get_name)
    ;
}
#else


HL_PRIM int HL_NAME(skeleton_num_joints)(ozz::animation::Skeleton* skeleton) {
	return skeleton->num_joints();
}

HL_PRIM int HL_NAME(skeleton_num_soa_joints)(ozz::animation::Skeleton* skeleton) {
	return skeleton->num_soa_joints();
}

DEFINE_PRIM(_I32, skeleton_num_joints, _STRUCT);
DEFINE_PRIM(_I32, skeleton_num_soa_joints, _STRUCT);



#endif