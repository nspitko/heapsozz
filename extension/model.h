
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
		printf("Init\n");
	}



	bool runSamplingJob( ozz::animation::SamplingJob *job );

	float *m_pSkinMatrixBuffer = nullptr;

	#ifdef EMSCRIPTEN
	bool loadMeshes(std::string data, int len);
	bool loadSkeleton(std::string data, int len);

	emscripten::val getSkinMatrices( int meshIndex );
	std::vector<Mesh *> getMeshes();
	#else
	bool loadMeshes(vbyte *data, int len);
	bool loadSkeleton(vbyte *data, int len);

	varray* getMeshes();
	vbyte* getSkinMatrices( int meshIndex );
	#endif

	ozz::animation::Skeleton* getSkeleton() { return &skeleton; }


	// Data
	ozz::vector<Mesh> meshes;
	ozz::animation::Skeleton skeleton;

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

