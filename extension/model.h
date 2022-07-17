#pragma once

#include "mesh.h"
#include "skeleton.h"

#include "ozz/base/containers/vector.h"
#include "ozz/base/span.h"
#include "ozz/base/maths/soa_transform.h"
#include "ozz/base/maths/soa_float4x4.h"
#include "ozz/animation/offline/raw_skeleton.h"
#include "ozz/animation/runtime/animation.h"
#include "ozz/animation/runtime/local_to_model_job.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/animation/runtime/track.h"
#include "ozz/animation/runtime/sampling_job.h"
#include "ozz/animation/offline/raw_skeleton.h"
#include "ozz/animation/runtime/skeleton.h"




/**
 * The purpose of ModelState is to contain all the little things we need to worry about
 * to render a model. Rather than trying to make heaps aware of all the simd-ridden
 * math and matrices, it's easier (and faster) to just leave it all over here and simply
 * keep it away from haxe entirely beyond a high level ref and some accessors.
 */
struct Model
{
	Model( ) : context(), meshes(), localTransforms(), modelMatrices(), skinningMatrices()
	{
		#ifndef EMSCRIPTEN
		hl_add_root( &skeleton );
		hl_add_root( &meshes );
		#endif
	}

	~Model()
	{
		#ifndef EMSCRIPTEN
		hl_remove_root( &skeleton );
		hl_remove_root( &meshes );
		#endif

		if( skinningMatrixBuffer != nullptr )
			free( skinningMatrixBuffer );

		printf("Model object has been finalized.\n");
	}


	bool runSamplingJob( ozz::animation::SamplingJob *job );

	#ifdef EMSCRIPTEN
	emscripten::val getSkinMatrices( int meshIndex );

	//
	std::vector<Mesh *> meshes;
	ozz::animation::Skeleton* skeleton;

	#else
	vbyte* getSkinMatrices( int meshIndex );

	//
	varray* meshes;
	hl_skeleton* skeleton;

	#endif



	int spacer = 10; // Debugging: Make sure we're aligned properly.

	// BELOW HERE: data structures which are not mapped to HL.

	float *skinningMatrixBuffer = nullptr;

	// Buffer of local model space matrices.
	ozz::vector<ozz::math::SoaTransform> localTransforms;

	// Buffer of model space matrices.
  	ozz::vector<ozz::math::Float4x4> modelMatrices;

	// Buffer of skinning matrices, result of the joint multiplication of the
	// inverse bind pose with the model space matrix.
	ozz::vector<ozz::math::Float4x4> skinningMatrices;

	// Sampling context.
	ozz::animation::SamplingJob::Context context;
};

