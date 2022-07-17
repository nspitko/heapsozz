package ozz;


import haxe.io.Bytes;


/**
 * Ozz animation externs for HL/JS.
 *
 * Bindings should be call comaptible between targets wherever possible.
 */

#if js
import js.lib.Uint8Array;

@:native("ozz.SamplingJob")
extern class SamplingJob
{
	public function new(): Void;
	public function validate(): Bool;
	public function setAnimation( animation: Animation ): Void;


	public var ratio: Float;
}


@:native("ozz.Part")
extern class Part
{


}

@:native("ozz.VectorMeshPtr")
extern class VectorMeshPtr
{
	public function get( idx: Int ) : Mesh;
	public function size(): Int;
	public function push_back(m: Mesh): Void;

	public var length(get, never): Int;
	public inline function get_length(): Int { return size(); }
}

@:native("ozz.Mesh")
extern class Mesh
{

	public function new();

	public inline function getVertexBuffer( stride: Int ) : js.lib.Uint8Array { return getVertexBufferImpl(); };
	public function getIndices() : js.lib.Uint16Array;
	public inline function load( data: Bytes, len: Int ): Int { return loadImpl( @:privateAccess data.b, len ); };

	public var triangleIndexCount : Int;
	public var vertexCount : Int;
	public var maxInfluencesCount : Int;
	public var skinned : Bool;
	public var jointCount : Int;
	public var highestJointIndex : Int;

	public var triangleIndexes : Array<Int>;
	public var parts : Array<Part>;
	public var partsCount : Int;

	function getVertexBufferImpl() : js.lib.Uint8Array;
	function loadImpl( data: js.lib.Uint8Array, len: Int ): Int;



}


@:native("ozz.Skeleton")
extern class Skeleton {

	public function new();

	public inline function load( data: Bytes, len: Int ): Bool { return loadImpl( @:privateAccess data.b, len ); };

	public var numJoints : Int;
	public var numSoaJoints : Int;
	public var jointParents : Array<Int>;

	function loadImpl( data: js.lib.Uint8Array, len: Int ): Bool;

}

@:native("ozz.Model")
extern class Model {
	public function new(): Void;

	public var meshes(get, set): VectorMeshPtr;
	public var skeleton(never, set): Skeleton;

	public inline function get_meshes( ): VectorMeshPtr { return getMeshes(); }
	public inline function set_meshes( v: VectorMeshPtr ): VectorMeshPtr { setMeshes(v); return v; }
	public inline function set_skeleton( v: Skeleton ): Skeleton { setSkeleton(v); return v; }

	public inline function getSkinMatrices( modelIndex: Int, jointCount: Int ) : js.lib.Float32Array { return getSkinMatricesImpl( modelIndex ); };

	public function getSkeleton(): Skeleton;
	public function getMeshes(): VectorMeshPtr;
	public function runSamplingJob( samplingJob: SamplingJob ): Bool;
	//
	public var duration : Float;
	public var trackCount : Int;
	public var soaTrackCount : Int;
	//
	function loadMeshesImpl( data: js.lib.Uint8Array, len: Int ): Bool;
	function loadSkeletonImpl( data: js.lib.Uint8Array, len: Int ): Bool;
	function getSkinMatricesImpl( modelIndex: Int ) : js.lib.Float32Array;
	function setMeshes( meshes: VectorMeshPtr ) : Void;
	function setSkeleton( skeleton: Skeleton ) : Void;

}

@:native("ozz.Animation")
extern class Animation {
	public function new(): Void;
	public inline function load( data: Bytes, len: Int ): Bool { return loadImpl( @:privateAccess data.b, len ); };

	public var duration : Float;
	public var trackCount : Int;
	public var soaTrackCount : Int;
	public function name() : String;

	function loadImpl( data: js.lib.Uint8Array, len: Int ): Bool;

}

class Init {
	public static function init(onReady:Void -> Void) {
		untyped js.Syntax.code('ozz().then(function(mod) { ozz = mod; onReady(); });');
	}
}

#else
typedef HLFinalizer = hl.Abstract<"finalizer">; // Used for constructs that require finalizer being called when GCd.


@:keep
@:hlNative("ozz")
@:struct class SamplingJob
{
	public function new() {	sampling_job_new(this); }

	public var ratio: Single; // Struct aligned
	public var animation: Animation; // Struct aligned

	public function validate(): Bool { return sampling_job_validate(this); }
	public function setAnimation( animation: Animation ): Void { sampling_job_setAnimation(this, animation); }

	static function sampling_job_new(samplingJob: SamplingJob): Void { }
	static function sampling_job_validate(samplingJob: SamplingJob): Bool { return false; }
	static function sampling_job_setAnimation(samplingJob: SamplingJob, animation: Animation): Void { }
}


@:struct private class AnimationStruct {
	@:noCompletion var finalizer: HLFinalizer; // [HL] GC finalizer
}

@:keep
@:hlNative("ozz") @:forward
abstract Animation(AnimationStruct) from AnimationStruct to AnimationStruct
{
	public inline function new() this = animation_init();

	public var duration(get, never): Single;
	public var trackCount(get, never): Int;
	public var soaTrackCount(get, never): Int;

	public inline function get_duration() { return animation_get_duration( this ); }
	public inline function get_trackCount() { return animation_get_track_count( this ); }
	public inline function get_soaTrackCount() { return animation_get_soa_track_count( this ); }

	public inline function load(data: Bytes, len: Int) : Bool { return animation_load(this, data, len); }

	static function animation_init(): AnimationStruct {return null;}
	static function animation_load(animation: Animation, data: hl.Bytes, len: Int): Bool { return false; }
	static function animation_get_duration(animation: Animation): Single { return 0; }
	static function animation_get_track_count(animation: Animation): Int { return 0; }
	static function animation_get_soa_track_count(animation: Animation): Int { return 0; }

}

@:keep
@:hlNative("ozz")
@:struct class Part
{
}

@:struct private class MeshStruct {
	@:noCompletion var finalizer: HLFinalizer; // [HL] GC finalizer
}

@:keep
@:hlNative("ozz") @:forward
abstract Mesh(MeshStruct) from MeshStruct to MeshStruct
{
	public inline function new() this = mesh_init();

	public var triangleIndexCount(get, never): Int;
	public var vertexCount(get, never): Int;
	public var maxInfluencesCount(get, never): Int;
	public var skinned(get, never): Bool;
	public var jointCount(get, never): Int;
	public var highestJointIndex(get, never): Int;
	public var partsCount(get, never): Int;

	public inline function load(data: Bytes, len: Int): Int { return mesh_load(this, data, len); }

	public inline function get_triangleIndexCount() { return mesh_triangle_index_count(this); }
	public inline function get_vertexCount() { return mesh_vertex_count(this); }
	public inline function get_maxInfluencesCount() { return mesh_max_influences_count(this); }
	public inline function get_skinned() { return mesh_skinned(this); }
	public inline function get_jointCount() { return mesh_joint_count(this); }
	public inline function get_highestJointIndex() { return mesh_highest_joint_index(this); }
	public inline function get_partsCount() { return mesh_parts_count(this); }

	public inline function getVertexBuffer(stride: Int): Bytes { return mesh_get_vertex_buffer(this).toBytes( vertexCount * stride ); }
	public inline function getIndices(): Bytes { return mesh_get_indices(this).toBytes(triangleIndexCount * 2); }

	static function mesh_init(): MeshStruct {return null;}
	static function mesh_load( mesh: Mesh, data: hl.Bytes, len: Int ): Int { return 0; }
	static function mesh_get_vertex_buffer( mesh: Mesh ): hl.Bytes { return null; }
	static function mesh_get_indices( mesh: Mesh ): hl.Bytes { return null; }
	static function mesh_triangle_index_count( mesh: Mesh ): Int { return 0; }
	static function mesh_vertex_count( mesh: Mesh ): Int { return 0; }
	static function mesh_max_influences_count( mesh: Mesh ): Int { return 0; }
	static function mesh_skinned( mesh: Mesh ): Bool { return false; }
	static function mesh_joint_count( mesh: Mesh ): Int { return 0; }
	static function mesh_highest_joint_index( mesh: Mesh ): Int { return 0; }
	static function mesh_parts_count( mesh: Mesh ): Int { return 0; }

}

@:struct private class SkeletonStruct {
	@:noCompletion var finalizer: HLFinalizer; // [HL] GC finalizer
}

@:keep
@:hlNative("ozz") @:forward
abstract Skeleton(SkeletonStruct) from SkeletonStruct to SkeletonStruct
{
	public inline function new() this = skeleton_init();

	public var numJoints(get, never): Int;
	public var numSoaJoints(get, never): Int;

	public inline function load(data: Bytes, len: Int): Bool { return skeleton_load(this, data, len); }

	public inline function get_numJoints() { return skeleton_num_joints(this); }
	public inline function get_numSoaJoints() { return skeleton_num_soa_joints(this); }

	static function skeleton_init(): SkeletonStruct {return null;}
	static function skeleton_num_joints(skeleton: Skeleton): Int { return 0; }
	static function skeleton_num_soa_joints(skeleton: Skeleton): Int { return 0; }
	static function skeleton_load( skeleton: Skeleton, data: hl.Bytes, len: Int ): Bool { return false; }

}

@:struct private class ModelStruct {

	@:noCompletion var finalizer: HLFinalizer; // [HL] GC finalizer

	public var meshes: hl.NativeArray<Mesh>;
	public var skeleton: Skeleton;

	// @todo delete
	public var spacer1: Int;

}

@:keep
@:hlNative("ozz", "model_") @:forward
abstract Model(ModelStruct) from ModelStruct to ModelStruct
{
	public inline function new() this = init();


	public inline function getSkinMatrices( meshIndex: Int, numMatrices: Int ) : Bytes { return get_skin_matrices(this, meshIndex).toBytes( numMatrices * 4 * 4 ); }
	public inline function runSamplingJob( job: SamplingJob ) : Bool { return run_sampling_job(this, job); }

	static function init(): ModelStruct {return null;}
	static function get_skin_matrices(model: Model, meshIndex: Int): hl.Bytes { return null; }
	static function run_sampling_job(model: Model, job: SamplingJob): Bool { return false; }
}

class Init
{
	public static function init(onReady:Void -> Void) {
		onReady();
	}
}





#end
