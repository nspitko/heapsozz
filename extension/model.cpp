#include "model.h"
#include "mesh.h"

#include "utils.h"

#include "ozz/base/containers/vector_archive.h"
#include "ozz/base/io/archive.h"
#include "ozz/base/io/stream.h"
#include "ozz/base/memory/allocator.h"
#include "ozz/base/maths/math_archive.h"
#include "ozz/base/maths/simd_math_archive.h"



bool Model::runSamplingJob( ozz::animation::SamplingJob *job )
{

	// Verify we have sized our datastructures enough to sample this job.
	#if EMSCRIPTEN
	const int num_soa_joints = skeleton->num_soa_joints();
    const int num_joints = skeleton->num_joints();
	#else
	const int num_soa_joints = skeleton->skeleton.num_soa_joints();
    const int num_joints = skeleton->skeleton.num_joints();
	#endif

	assert(num_joints > 0);

	localTransforms.resize(num_soa_joints);
    modelMatrices.resize(num_joints);

	// Allocates a context that matches animation requirements.
	// @todo: We shouldn't do this every frame! Only when the skeleton changes.
	context.Resize(num_joints);

	job->context = &context;
	job->output = ozz::make_span(localTransforms);



	// Ensure we have enough skin matrices
	size_t num_skinning_matrices = 0;
	#if EMSCRIPTEN
	for (int i=0; i<meshes.size(); i++)
	{
		Mesh* m = meshes[i];
    	num_skinning_matrices = ozz::math::Max(num_skinning_matrices, m->joint_remaps.size());
    }
	#else
    for (int i=0; i<meshes->size; i++)
	{
		hl_mesh* m = hl_aptr(meshes,hl_mesh*)[i];
    	num_skinning_matrices = ozz::math::Max(num_skinning_matrices, m->mesh.joint_remaps.size());
    }
	#endif

    // Allocates skinning matrices.
    skinningMatrices.resize(num_skinning_matrices);


	if( !job->Run() )
		return false;

	// Converts from local space to model space matrices.
    ozz::animation::LocalToModelJob ltm_job;
	#if EMSCRIPTEN
	ltm_job.skeleton = skeleton;
	#else
    ltm_job.skeleton = &skeleton->skeleton;
	#endif
    ltm_job.input = make_span(localTransforms);
    ltm_job.output = make_span(modelMatrices);
    if (!ltm_job.Run()) {
      return false;
    }

	return true;

}


#ifdef EMSCRIPTEN
emscripten::val Model::getSkinMatrices( int meshIndex )
#else
vbyte* Model::getSkinMatrices( int meshIndex )
#endif
{
	// First, apply skinning matrices (use joint remaps and multiply by inverse bind pose)
#if EMSCRIPTEN
	Mesh* mesh = meshes[meshIndex];
#else
	Mesh *mesh = &hl_aptr(meshes,hl_mesh*)[meshIndex]->mesh;
#endif

	int numSkinJoints = mesh->num_joints();


	int size = numSkinJoints * (4*4);
	if( skinningMatrixBuffer == nullptr )
		skinningMatrixBuffer = (float *)malloc( sizeof(float) * size );

	float* farr = skinningMatrixBuffer;

	int a=0;

	for (int i = 0; i < numSkinJoints; i++)
	{
		ozz::math::Float4x4 skin_matrix = modelMatrices[mesh->joint_remaps[i]] * mesh->inverse_bind_poses[i];
		const ozz::math::SimdFloat4& c0 = skin_matrix.cols[0];
		const ozz::math::SimdFloat4& c1 = skin_matrix.cols[1];
		const ozz::math::SimdFloat4& c2 = skin_matrix.cols[2];
		const ozz::math::SimdFloat4& c3 = skin_matrix.cols[3];

		// @todo I don't think we need to send the full matrix? Can probably get away with
		// 4x3 here if we initialize the target matrices properly.

		// @todo would be nice if we could just copy these in one pass instead of four gets;
		// maybe some assumptions we can do with the SIMD structures?

		farr[a++] = ozz::math::GetX( c0 );
		farr[a++] = ozz::math::GetY( c0 );
		farr[a++] = ozz::math::GetZ( c0 );
		farr[a++] = ozz::math::GetW( c0 );

		farr[a++] = ozz::math::GetX( c1 );
		farr[a++] = ozz::math::GetY( c1 );
		farr[a++] = ozz::math::GetZ( c1 );
		farr[a++] = ozz::math::GetW( c1 );

		farr[a++] = ozz::math::GetX( c2 );
		farr[a++] = ozz::math::GetY( c2 );
		farr[a++] = ozz::math::GetZ( c2 );
		farr[a++] = ozz::math::GetW( c2 );

		farr[a++] = ozz::math::GetX( c3 );
		farr[a++] = ozz::math::GetY( c3 );
		farr[a++] = ozz::math::GetZ( c3 );
		farr[a++] = ozz::math::GetW( c3 );

	}

#ifdef EMSCRIPTEN
	return emscripten::val(emscripten::typed_memory_view(size, skinningMatrixBuffer));
#else
	return (vbyte*)skinningMatrixBuffer;
#endif
}


#ifdef EMSCRIPTEN

using namespace emscripten;

const static ozz::animation::Skeleton* model_get_skeleton(const Model &ms2) {
    // Return a MyStruct by-val to be converted into a value object.
    return ms2.skeleton;
}

static void model_set_skeleton(Model &model, ozz::animation::Skeleton *skeleton) {
    model.skeleton = skeleton;
}


static void model_set_meshes(Model &model,  std::vector<Mesh *> meshes) {
    model.meshes = meshes;
}

const static std::vector<Mesh *> model_get_meshes(Model &model) {
    return model.meshes;
}



EMSCRIPTEN_BINDINGS(ozzModel) {

	register_vector<Mesh *>("VectorMeshPtr");

	class_<Model>("Model")
		.constructor<>()
		//

		.function("getSkinMatricesImpl", &Model::getSkinMatrices)
		.function("runSamplingJob", &Model::runSamplingJob, allow_raw_pointers())
		.function("setSkeleton", &model_set_skeleton, allow_raw_pointers())
		.function("setMeshes", &model_set_meshes, allow_raw_pointers())
		.function("getMeshes", &model_get_meshes, allow_raw_pointers())
		//
		//.property("meshes" &Model::meshes)
		//.property("skeleton", &model_get_skeleton, &model_set_skeleton)
		//.field("meshes", &Model::meshes)
		//.field("skeleton", &Model::skeleton, allow_raw_pointers())
		//.field("localTransforms", &Model::localTransforms)
		//.field("modelMatrices", &Model::modelMatrices)
		//.field("skinningMatrices", &Model::skinningMatrices)


		//.class_property("name", &animation_get_name)
    ;
}
#else

typedef struct _hl_model hl_model;
struct _hl_model {
	void(*finalize)(hl_model*);
	Model model;
};

static void model_finalize(hl_model *m)
{
	m->model.~Model();
}

/**
 * We use this to allocate the object with a finalizer; since Haxe doesn't have native
 * destructors, this is the only way to currently have this kind of behavior.
 */
HL_PRIM hl_model* HL_NAME(model_init)()
{
	hl_model* hl_mem = (hl_model*)hl_gc_alloc_finalizer(sizeof(hl_model));
	hl_mem->finalize = model_finalize;
	new (&hl_mem->model)Model();
	return hl_mem;
}

HL_PRIM vbyte* HL_NAME(model_get_skin_matrices)(hl_model* m, int meshIndex) {
	return m->model.getSkinMatrices( meshIndex );
}

HL_PRIM bool HL_NAME(model_run_sampling_job)(hl_model* m, ozz::animation::SamplingJob* job) {
	return m->model.runSamplingJob( job );
}


DEFINE_PRIM(_STRUCT, model_init, _NO_ARG);
DEFINE_PRIM(_BYTES, model_get_skin_matrices, _STRUCT _I32 );
DEFINE_PRIM(_BOOL, model_run_sampling_job, _STRUCT _STRUCT );


#endif