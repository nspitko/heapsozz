
#include "utils.h"

#include "ozz/base/containers/vector.h"
#include "ozz/base/span.h"
#include "ozz/animation/offline/raw_skeleton.h"
#include "ozz/animation/runtime/animation.h"
#include "ozz/animation/runtime/local_to_model_job.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/animation/runtime/track.h"

#ifdef EMSCRIPTEN



#else

typedef struct _hl_skeleton hl_skeleton;
struct _hl_skeleton {
	void(*finalize)(hl_skeleton*);
	ozz::animation::Skeleton skeleton;
};

bool load_skeleton(ozz::animation::Skeleton *skeleton, vbyte* data, int len);

#endif

int skeleton_num_joint_names( ozz::animation::Skeleton *skeleton );
vbyte *skeleton_get_joint_name(ozz::animation::Skeleton *skeleton, int idx);