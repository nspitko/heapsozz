

#include "animation.h"

#include "ozz/base/io/archive.h"
#include "ozz/base/io/stream.h"

#ifdef EMSCRIPTEN
bool animationLoad(ozz::animation::Animation &animation, std::string data, int len)
#else
bool animationLoad(ozz::animation::Animation &animation, vbyte *data, int len)
#endif
{
	printf("Loading animation archive\n" );


	ozz::io::MemoryStream ms;
#ifdef EMSCRIPTEN
	ms.Write( data.c_str(), len );
#else
	ms.Write( data, len );
#endif
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

std::string animationGetName(const ozz::animation::Animation &animation)
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
		.function("loadImpl", &animationLoad)
		.property("duration", &ozz::animation::Animation::duration)
		.property("trackCount", &ozz::animation::Animation::num_tracks)
		.property("soaTrackCount", &ozz::animation::Animation::num_soa_tracks)
		//.class_property("name", &animation_get_name)
    ;
}

#else

static void animation_finalize(hl_animation *a)
{
	a->animation.~Animation();
}

HL_PRIM hl_animation* HL_NAME(animation_init)()
{
	hl_animation* hl_mem = (hl_animation*)hl_gc_alloc_finalizer(sizeof(hl_animation));
	hl_mem->finalize = animation_finalize;
	new (&hl_mem->animation)ozz::animation::Animation();
	return hl_mem;
}

HL_PRIM bool HL_NAME(animation_load)(hl_animation* a, vbyte* data, int len)
{
	return animationLoad(a->animation, data, len);
}

HL_PRIM float HL_NAME(animation_get_duration)(hl_animation* a)
{
	return a->animation.duration();
}

HL_PRIM int HL_NAME(animation_get_track_count)(hl_animation* a)
{
	return a->animation.num_tracks();
}

HL_PRIM int HL_NAME(animation_get_soa_track_count)(hl_animation* a)
{
	return a->animation.num_soa_tracks();
}

DEFINE_PRIM(_STRUCT, animation_init, _NO_ARG );
DEFINE_PRIM(_BOOL, animation_load, _STRUCT _BYTES _I32);
DEFINE_PRIM(_F32, animation_get_duration, _STRUCT );
DEFINE_PRIM(_I32, animation_get_track_count, _STRUCT );
DEFINE_PRIM(_I32, animation_get_soa_track_count, _STRUCT );

#endif