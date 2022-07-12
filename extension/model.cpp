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
	job->context = &context;
	job->output = ozz::make_span( localTransforms );

	if( !job->Run() )
		return false;

	// Converts from local space to model space matrices.
    ozz::animation::LocalToModelJob ltm_job;
    ltm_job.skeleton = &skeleton;
    ltm_job.input = make_span(localTransforms);
    ltm_job.output = make_span(modelMatrices);
    if (!ltm_job.Run()) {
      return false;
    }

	return true;

}

// @todo: Currently we don't share skeletons/meshes between instances. This is pretty bad!
#ifdef EMSCRIPTEN
bool Model::loadMeshes(std::string data, int len)
#else
bool Model::loadMeshes(vbyte *data, int len)
#endif
{
	ozz::io::MemoryStream ms;
	#ifdef EMSCRIPTEN
	ms.Write( data.data(), len );
	#else
	ms.Write( data, len );
	#endif
	ms.Seek(0,ms.kSet);

	ozz::io::IArchive archive(&ms);


	if (!archive.TestTag<Mesh>())
	{
		printf("Failed to load mesh instance from file\n");
		return false;
	}

	while (archive.TestTag<Mesh>())
	{
		meshes.resize(meshes.size() + 1);
		archive >> meshes.back();
	}

	// Ensure we have enough skin matrices
	size_t num_skinning_matrices = 0;
    for (const Mesh& mesh : meshes) {
    	num_skinning_matrices = ozz::math::Max(num_skinning_matrices, mesh.joint_remaps.size());
    }

    // Allocates skinning matrices.
    skinningMatrices.resize(num_skinning_matrices);


	return true;
}


#ifdef EMSCRIPTEN
bool Model::loadSkeleton(std::string data, int len)
#else
bool Model::loadSkeleton(vbyte *data, int len)
#endif
{
	ozz::io::MemoryStream ms;
	#ifdef EMSCRIPTEN
	ms.Write( data.data(), len );
	#else
	ms.Write( data, len );
	#endif

	ms.Seek(0,ms.kSet);

	ozz::io::IArchive archive(&ms);


	if (!archive.TestTag<ozz::animation::Skeleton>())
	{
		printf("Failed to load mesh instance from file\n");
		return false;
	}

	archive >> skeleton;

	const int num_soa_joints = skeleton.num_soa_joints();
    localTransforms.resize(num_soa_joints);
    const int num_joints = skeleton.num_joints();
    modelMatrices.resize(num_joints);

    // Allocates a context that matches animation requirements.
    context.Resize(num_joints);

	return true;
}

#ifdef EMSCRIPTEN
emscripten::val Model::getSkinMatrices( int meshIndex )
#else
vbyte* Model::getSkinMatrices( int meshIndex )
#endif
{
	// First, apply skinning matrices (use joint remaps and multiply by inverse bind pose)
	Mesh *mesh = &meshes[meshIndex];

	int numSkinJoints = mesh->joint_count();


	int size = numSkinJoints * (4*4);
	if( m_pSkinMatrixBuffer == nullptr )
		m_pSkinMatrixBuffer = (float *)malloc( sizeof(float) * size );

	float* farr = m_pSkinMatrixBuffer;

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
	return emscripten::val(emscripten::typed_memory_view(size, m_pSkinMatrixBuffer));;
#else
	return (vbyte*)m_pSkinMatrixBuffer;
#endif
}

#ifndef EMSCRIPTEN
varray* Model::getMeshes()
{

	varray *arr = hl_alloc_array(&hlt_abstract, (int)meshes.size());
	for( size_t i=0; i<meshes.size();i++)
	{
		hl_aptr(arr,Mesh*)[i] = &meshes.at(i);
	}

	return arr;
}
#else
std::vector<Mesh *> Model::getMeshes()
{
	std::vector<Mesh *> vec;
	for( int i=0; i<meshes.size(); i++)
		vec.push_back( &meshes[i]);


	return vec;
}

#endif

#ifdef EMSCRIPTEN

using namespace emscripten;


EMSCRIPTEN_BINDINGS(ozzModel) {

	register_vector<Mesh *>("VectorMeshPtr");

	class_<Model>("Model")
		.constructor<>()
		//
		.function("loadMeshesImpl", &Model::loadMeshes)
		.function("loadSkeletonImpl", &Model::loadSkeleton)
		.function("getSkeleton", &Model::getSkeleton, allow_raw_pointers())
		.function("getMeshes", &Model::getMeshes, allow_raw_pointers())
		.function("getSkinMatricesImpl", &Model::getSkinMatrices)
		.function("runSamplingJob", &Model::runSamplingJob, allow_raw_pointers())
		//
		//.property("meshes")
		//.field("meshes", &Model::meshes)
		//.field("skeleton", &Model::skeleton, allow_raw_pointers())
		//.field("localTransforms", &Model::localTransforms)
		//.field("modelMatrices", &Model::modelMatrices)
		//.field("skinningMatrices", &Model::skinningMatrices)


		//.class_property("name", &animation_get_name)
    ;
}
#else

HL_PRIM void HL_NAME(model_new)(Model* model)
{
	if (model != nullptr) new (model)Model(); else printf("Failed to init struct; ptr is null\n");
}

HL_PRIM bool HL_NAME(model_load_meshes)(Model* model, vbyte* data, int length) {
	return model->loadMeshes(data, length);
}

HL_PRIM bool HL_NAME(model_load_skeleton)(Model* model, vbyte* data, int length) {
	return model->loadSkeleton(data, length);
}

HL_PRIM ozz::animation::Skeleton* HL_NAME(model_get_skeleton)(Model* model ) {
	return model->getSkeleton();
}

HL_PRIM varray* HL_NAME(model_get_meshes)(Model* model ) {
	return model->getMeshes();
}

HL_PRIM vbyte* HL_NAME(model_get_skin_matrices)(Model* model, int meshIndex) {
	return model->getSkinMatrices( meshIndex );
}

HL_PRIM bool HL_NAME(model_run_sampling_job)(Model* model, ozz::animation::SamplingJob* job) {
	return model->runSamplingJob( job );
}

DEFINE_PRIM(_VOID, model_new, _STRUCT);
DEFINE_PRIM(_BOOL, model_load_meshes, _STRUCT _BYTES _I32 );
DEFINE_PRIM(_BOOL, model_load_skeleton, _STRUCT _BYTES _I32);
DEFINE_PRIM(_STRUCT, model_get_skeleton, _STRUCT );
DEFINE_PRIM(_ARR, model_get_meshes, _STRUCT );
DEFINE_PRIM(_BYTES, model_get_skin_matrices, _STRUCT _I32 );
DEFINE_PRIM(_BOOL, model_run_sampling_job, _STRUCT _STRUCT );


#endif