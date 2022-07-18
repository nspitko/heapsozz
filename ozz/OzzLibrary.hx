package ozz;

import hxd.res.Resource;
import hxd.fs.FileEntry;
import ozz.Ozz.Model;
import ozz.Ozz.Mesh;
import ozz.Ozz.Skeleton;
import ozz.Ozz.Animation;

#if js
import ozz.Ozz.VectorMeshPtr;
#end

/**
 * OzzLibrary exists to hold and cache all ozz data for future use
 *
 * It apes from Heaps' HMD library class, but mostly exists as a way to simplify the
 * API and solve some memory manage issues with current JS
 */
class OzzLibrary
{

	public var meshes: Map<String,Array<Mesh>> = [];
	public var skeletons: Map<String,Skeleton> = [];
	// Models are a combination of a mesh group and skeleton.
	// We make a unique model per combo for caching reasons, but it
	// may be better to just set the skeleton during sampling instead?
	public var models: Map<String,Model> = [];

	// Animations are not model specific
	public var animations: Map<String,Animation> = [];

	public function new()
	{

	}

	function loadMesh(res: Resource)
	{
		var key = res.name;
		if( meshes.exists( key) )
			return meshes[key];

		var meshGroup = [];

		var bytes = res.entry.getBytes();
		var window = bytes;
		var pos = 0;

		while( pos < bytes.length )
		{
			var mesh = new ozz.Ozz.Mesh();
			pos = mesh.load(window, bytes.length);
			trace(pos);
			if( pos == 0 )
				break;
			meshGroup.push( mesh );
			window = bytes.sub(pos, bytes.length - pos);
		}

		meshes.set(key, meshGroup);

		return meshGroup;
	}

	function loadSkeleton(res: Resource)
	{
		var key = res.name;
		if( skeletons.exists( key) )
			return skeletons[key];

		var skeleton = new Skeleton();

		var bytes = res.entry.getBytes();
		skeleton.load( bytes, bytes.length );

		skeletons.set(key, skeleton);

		return skeleton;
	}

	public function loadAnimation(res: Resource)
	{
		var key = res.name;
		if( animations.exists( key) )
			return animations[key];

		var animation = new Animation();

		var bytes = res.entry.getBytes();
		animation.load( bytes, bytes.length );

		animations.set(key, animation);

		return animation;
	}

	public function loadModel( meshRes: Resource, skeletonRes: Resource )
	{
		var meshKey = meshRes.name;
		var skeletonKey = skeletonRes.name;
		var key = '$meshKey-$skeletonKey';

		if( models.exists( key) )
			return models[key];

		var model = new Ozz.Model();

		var meshes = loadMesh(meshRes);

		#if hl
		var nativeMeshes = new hl.NativeArray<Mesh>( meshes.length );
		for( i in 0 ... meshes.length )
			nativeMeshes[i] = meshes[i];

		model.meshes = nativeMeshes;
		#else
		var tmpArray: VectorMeshPtr = js.Syntax.code("new ozz.VectorMeshPtr();");
		for( i in 0 ... meshes.length )
			tmpArray.push_back(meshes[i]);
		model.meshes = tmpArray;
		#end

		model.skeleton = loadSkeleton(skeletonRes);

		models.set(key,model);

		return model;

	}

	// Clears all cached model refs
	// WARNING: This deletes the memory for all active models.
	// If you try to render a model after it has been deleted
	// you will absolutely crash on JS!
	public function dispose()
	{
		#if js
		for( m in models )
			js.Syntax.code('{0}.delete();',m);
		for( m in meshes )
			js.Syntax.code('{0}.delete();',m);
		for( m in skeletons )
			js.Syntax.code('{0}.delete();',m);
		for( m in animations )
			js.Syntax.code('{0}.delete();',m);

		#end
	}
}