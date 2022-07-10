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

vbyte *skeleton_get_joint_name(ozz::animation::Skeleton *skeleton, int idx)
{
	return const_char_to_vbytes( skeleton->joint_names()[idx] );
}
