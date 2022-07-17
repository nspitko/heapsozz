#include "utils.h"

#include "ozz/base/containers/vector.h"
#include "ozz/base/span.h"
#include "ozz/animation/offline/raw_skeleton.h"
#include "ozz/animation/runtime/animation.h"
#include "ozz/animation/runtime/local_to_model_job.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/animation/runtime/track.h"


#ifdef EMSCRIPTEN
bool animation_load(ozz::animation::Animation& animation, std::string data, int len);
std::string animation_get_name(ozz::animation::Animation *animation);
#else
typedef struct _hl_animation hl_animation;
struct _hl_animation {
	void(*finalize)(hl_animation*);
	ozz::animation::Animation animation;
};

bool load_animation(ozz::animation::Animation *animation, vbyte* data, int len);

#endif



