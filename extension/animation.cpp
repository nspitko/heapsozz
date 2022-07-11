

#include "animation.h"

#include "ozz/base/io/archive.h"
#include "ozz/base/io/stream.h"


bool animation_load(ozz::animation::Animation& animation, std::string data, int len)
{
	printf("Loading animation archive\n" );


	ozz::io::MemoryStream ms;
	ms.Write( data.c_str(), len );
	ms.Seek(0,ms.kSet);

	ozz::io::IArchive archive(&ms);

	if (!archive.TestTag<ozz::animation::Animation>())
	{
		printf("Failed to load animation from file\n");
		return false;
	}

	// Once the tag is validated, reading cannot fail.

	archive >> animation;

	return true;
}

std::string animation_get_name(const ozz::animation::Animation& animation)
{
	return std::string(animation.name());
}


namespace {
template <typename _Track>
bool LoadTrackImpl(_Track *track, char* data, int len)
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

bool LoadTrack(ozz::animation::FloatTrack* _track, char* data, int len) {
  return LoadTrackImpl(_track, data, len);
}
bool LoadTrack(ozz::animation::Float2Track* _track, char* data, int len) {
  return LoadTrackImpl(_track, data, len);
}
bool LoadTrack(ozz::animation::Float3Track* _track, char* data, int len) {
  return LoadTrackImpl(_track, data, len);
}
bool LoadTrack(ozz::animation::Float4Track* _track, char* data, int len) {
  return LoadTrackImpl(_track, data, len);
}
bool LoadTrack(ozz::animation::QuaternionTrack* _track, char* data, int len) {
  return LoadTrackImpl(_track, data, len);
}
} // namespace


#ifdef EMSCRIPTEN

using namespace emscripten;

EMSCRIPTEN_BINDINGS(ozzAnimation) {
	class_<ozz::animation::Animation>("Animation")
		.constructor<>()
		.class_function("load", &animation_load)
		.property("duration", &ozz::animation::Animation::duration)
		.property("trackCount", &ozz::animation::Animation::num_tracks)
		.property("soaTrackCount", &ozz::animation::Animation::num_soa_tracks)
		//.class_property("name", &animation_get_name)
    ;
}

#endif