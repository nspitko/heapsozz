

#include "animation.h"
#include "utils.h"

#include "ozz/base/io/archive.h"
#include "ozz/base/io/stream.h"


bool load_animation(ozz::animation::Animation *animation, vbyte* data, int len)
{
	printf("Loading animation archive\n" );


	ozz::io::MemoryStream ms;
	ms.Write( data, len );
	ms.Seek(0,ms.kSet);

	ozz::io::IArchive archive(&ms);

	if (!archive.TestTag<ozz::animation::Animation>())
	{
		printf("Failed to load animation from file\n");
		return false;
	}

	// Once the tag is validated, reading cannot fail.
	archive >> *animation;

	return true;
}


namespace {
template <typename _Track>
bool LoadTrackImpl(_Track *track, vbyte* data, int len)
{

	printf("Loading track\n" );


	ozz::io::MemoryStream ms;
	ms.Write( data, len );
	ms.Seek(0,ms.kSet);

	ozz::io::IArchive archive(&ms);

	if (!archive.TestTag<_Track>())
	{
		printf("Failed to load track\n");
		return false;
	}

	// Once the tag is validated, reading cannot fail.
	archive >> *track;

	return true;

}  // namespace

bool LoadTrack(ozz::animation::FloatTrack* _track, vbyte* data, int len) {
  return LoadTrackImpl(_track, data, len);
}
bool LoadTrack(ozz::animation::Float2Track* _track, vbyte* data, int len) {
  return LoadTrackImpl(_track, data, len);
}
bool LoadTrack(ozz::animation::Float3Track* _track, vbyte* data, int len) {
  return LoadTrackImpl(_track, data, len);
}
bool LoadTrack(ozz::animation::Float4Track* _track, vbyte* data, int len) {
  return LoadTrackImpl(_track, data, len);
}
bool LoadTrack(ozz::animation::QuaternionTrack* _track, vbyte* data, int len) {
  return LoadTrackImpl(_track, data, len);
}
} // namespace
