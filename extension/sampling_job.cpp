#include "utils.h"

#include "ozz/animation/runtime/sampling_job.h"
#include "ozz/animation/runtime/animation.h"


float getRatio( ozz::animation::SamplingJob* pJob ) { return pJob->ratio; }
void setRatio( ozz::animation::SamplingJob* pJob, float ratio ) { pJob->ratio = ratio; }

const ozz::animation::Animation *getAnimation( ozz::animation::SamplingJob &job ) { return job.animation; }
void setAnimation( ozz::animation::SamplingJob &job, ozz::animation::Animation *pAnimation ) { job.animation = pAnimation; }


#ifdef EMSCRIPTEN

using namespace emscripten;

EMSCRIPTEN_BINDINGS(ozzSamplingJob) {
	class_<ozz::animation::SamplingJob>("SamplingJob")
		.constructor<>()

		.function("validate", &ozz::animation::SamplingJob::Validate )
		.property("ratio", &ozz::animation::SamplingJob::ratio )
		.function("setAnimation", &setAnimation, allow_raw_pointers() );//, &setAnimation)
    ;
}

#endif