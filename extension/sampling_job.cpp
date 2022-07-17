#include "utils.h"
#include "animation.h"

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

#else


HL_PRIM void HL_NAME(sampling_job_new)(ozz::animation::SamplingJob* job)
{
	if (job != nullptr) new (job)ozz::animation::SamplingJob();
}

HL_PRIM bool HL_NAME(sampling_job_validate)(ozz::animation::SamplingJob* job)
{
	return job->Validate();
}

HL_PRIM void HL_NAME(sampling_job_set_animation)(ozz::animation::SamplingJob* job, hl_animation* a)
{
	job->animation = &a->animation;
}

DEFINE_PRIM(_VOID, sampling_job_new, _STRUCT);
DEFINE_PRIM(_BOOL, sampling_job_validate, _STRUCT);
DEFINE_PRIM(_VOID, sampling_job_set_animation, _STRUCT _STRUCT);

#endif