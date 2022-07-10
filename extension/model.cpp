#include "model.h"
#include "mesh.h"

#include <hl.h>
#include "utils.h"

#include "ozz/base/containers/vector_archive.h"
#include "ozz/base/io/archive.h"
#include "ozz/base/io/stream.h"
#include "ozz/base/memory/allocator.h"
#include "ozz/base/maths/math_archive.h"
#include "ozz/base/maths/simd_math_archive.h"


// Don't actually free the ref. (Yes this is dumb)
static void finalize_Mesh( _ref(Mesh)* _this ) {  }

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
bool Model::loadMeshes(vbyte* data, int len)
{
	ozz::io::MemoryStream ms;
	ms.Write( data, len );
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



bool Model::loadSkeleton(vbyte* data, int len)
{
	ozz::io::MemoryStream ms;
	ms.Write( data, len );
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


varray* Model::getMeshes()
{
	varray *arr = hl_alloc_array(&hlt_abstract, meshes.size());
	for( int i=0; i<meshes.size();i++)
	{
		hl_aptr(arr,_ref(Mesh)*)[i] = alloc_ref(&meshes.at(i), Mesh);


	}


	return arr;
}

varray* Model::getSkinMatrices( int meshIndex)
{
	// First, apply skinning matrices (use joint remaps and multiply by inverse bind pose)
	Mesh *mesh = &meshes[meshIndex];

	int numSkinJoints = mesh->num_joints();


	int size = numSkinJoints * (4*4);
	varray *arr = hl_alloc_array(&hlt_f32, size );

	float* farr = hl_aptr(arr,float);

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

		/*

		farr[a++] = ozz::math::GetX( c0 );
		farr[a++] = ozz::math::GetX( c1 );
		farr[a++] = ozz::math::GetX( c2 );
		farr[a++] = ozz::math::GetX( c3 );

		farr[a++] = ozz::math::GetY( c0 );
		farr[a++] = ozz::math::GetY( c1 );
		farr[a++] = ozz::math::GetY( c2 );
		farr[a++] = ozz::math::GetY( c3 );

		farr[a++] = ozz::math::GetZ( c0 );
		farr[a++] = ozz::math::GetZ( c1 );
		farr[a++] = ozz::math::GetZ( c2 );
		farr[a++] = ozz::math::GetZ( c3 );
		*/

	}

	return arr;
}