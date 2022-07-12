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
}

@:native("ozz.Mesh")
extern class Mesh
{

	public inline function getVertexBuffer( stride: Int ) : js.lib.Uint8Array { return getVertexBufferImpl(); };
	public function getIndices() : js.lib.Uint16Array;

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



}


@:native("ozz.Skeleton")
extern class Skeleton {

	public var numJoints : Int;
	public var numSoaJoints : Int;
	public var jointParents : Array<Int>;

}

@:native("ozz.Model")
extern class Model {
	public function new(): Void;
	public inline function loadMeshes( data: Bytes, len: Int ): Bool { return loadMeshesImpl( @:privateAccess data.b, len ); };
	public inline function loadSkeleton( data: Bytes, len: Int ): Bool { return loadSkeletonImpl( @:privateAccess data.b, len ); };
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

}

@:native("ozz.Animation")
extern class Animation {
	public function new(): Void;
	static public function load( animation: Animation, data: js.lib.Uint8Array, len: Int ): Void;
	public var duration : Float;
	public var trackCount : Int;
	public var soaTrackCount : Int;
	public function name() : String;
}

class Init {
	public static function init(onReady:Void -> Void) {
		untyped js.Syntax.code('ozz().then(function(mod) { ozz = mod; onReady(); });');
	}
}

#else

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

	private var padding1: Float;
	private var padding2: Float;
	private var padding3: Float;
	private var padding4: Float;
	private var padding5: Float;
	private var padding6: Float;
	private var padding7: Float;
	private var padding8: Float;
	private var padding9: Float;
	private var padding10: Float;
	private var padding11: Float;
	private var padding12: Float;
	private var padding13: Float;
	private var padding14: Float;
	private var padding15: Float;
	private var padding16: Float;
	private var padding17: Float;
	private var padding18: Float;
	private var padding19: Float;
	private var padding20: Float;
	private var padding21: Float;
	private var padding22: Float;
	private var padding23: Float;
	private var padding24: Float;
	private var padding25: Float;
	private var padding26: Float;
	private var padding27: Float;
	private var padding28: Float;
	private var padding29: Float;
	private var padding30: Float;



}

@:keep
@:hlNative("ozz")
@:struct class Animation
{
	public function new() {	animation_new(this); }

	public var duration(get, never): Single;
	public var trackCount(get, never): Int;
	public var soaTrackCount(get, never): Int;

	public inline function get_duration() { return animation_get_duration( this ); }
	public inline function get_trackCount() { return animation_get_track_count( this ); }
	public inline function get_soaTrackCount() { return animation_get_soa_track_count( this ); }

	public inline function load(data: Bytes, len: Int) : Bool { return animation_load(this, data, len); }

	static function animation_new(animation: Animation): Void {}
	static function animation_load(animation: Animation, data: hl.Bytes, len: Int): Bool { return false; }
	static function animation_get_duration(animation: Animation): Single { return 0; }
	static function animation_get_track_count(animation: Animation): Int { return 0; }
	static function animation_get_soa_track_count(animation: Animation): Int { return 0; }

	private var padding1: Float;
	private var padding2: Float;
	private var padding3: Float;
	private var padding4: Float;
	private var padding5: Float;
	private var padding6: Float;
	private var padding7: Float;
	private var padding8: Float;
	private var padding9: Float;
	private var padding10: Float;
	private var padding11: Float;
	private var padding12: Float;
	private var padding13: Float;
	private var padding14: Float;
	private var padding15: Float;
	private var padding16: Float;
	private var padding17: Float;
	private var padding18: Float;
	private var padding19: Float;
	private var padding20: Float;
	private var padding21: Float;
	private var padding22: Float;
	private var padding23: Float;
	private var padding24: Float;
	private var padding25: Float;
	private var padding26: Float;
	private var padding27: Float;
	private var padding28: Float;
	private var padding29: Float;
	private var padding30: Float;
	private var padding31: Float;
	private var padding32: Float;
	private var padding33: Float;
	private var padding34: Float;
	private var padding35: Float;
	private var padding36: Float;
	private var padding37: Float;
	private var padding38: Float;
	private var padding39: Float;
	private var padding40: Float;
	private var padding41: Float;
	private var padding42: Float;
	private var padding43: Float;
	private var padding44: Float;
	private var padding45: Float;
	private var padding46: Float;
}

@:keep
@:hlNative("ozz")
@:struct class Part
{
}

@:keep
@:hlNative("ozz")
@:struct class Mesh
{
	public var triangleIndexCount(get, never): Int;
	public var vertexCount(get, never): Int;
	public var maxInfluencesCount(get, never): Int;
	public var skinned(get, never): Bool;
	public var jointCount(get, never): Int;
	public var highestJointIndex(get, never): Int;
	public var partsCount(get, never): Int;

	public inline function get_triangleIndexCount() { return mesh_triangle_index_count(this); }
	public inline function get_vertexCount() { return mesh_vertex_count(this); }
	public inline function get_maxInfluencesCount() { return mesh_max_influences_count(this); }
	public inline function get_skinned() { return mesh_skinned(this); }
	public inline function get_jointCount() { return mesh_joint_count(this); }
	public inline function get_highestJointIndex() { return mesh_highest_joint_index(this); }
	public inline function get_partsCount() { return mesh_parts_count(this); }

	public inline function getVertexBuffer(stride: Int): Bytes { return mesh_get_vertex_buffer(this).toBytes( vertexCount * stride ); }
	public inline function getIndices(): Bytes { return mesh_get_indices(this).toBytes(triangleIndexCount * 2); }

	static function mesh_get_vertex_buffer( mesh: Mesh ): hl.Bytes { return null; }
	static function mesh_get_indices( mesh: Mesh ): hl.Bytes { return null; }
	static function mesh_triangle_index_count( mesh: Mesh ): Int { return 0; }
	static function mesh_vertex_count( mesh: Mesh ): Int { return 0; }
	static function mesh_max_influences_count( mesh: Mesh ): Int { return 0; }
	static function mesh_skinned( mesh: Mesh ): Bool { return false; }
	static function mesh_joint_count( mesh: Mesh ): Int { return 0; }
	static function mesh_highest_joint_index( mesh: Mesh ): Int { return 0; }
	static function mesh_parts_count( mesh: Mesh ): Int { return 0; }

	private var padding1: Float;
	private var padding2: Float;
	private var padding3: Float;
	private var padding4: Float;
	private var padding5: Float;
	private var padding6: Float;
}

@:keep
@:hlNative("ozz")
@:struct class Skeleton
{
	public var numJoints(get, never): Int;
	public var numSoaJoints(get, never): Int;

	public inline function get_numJoints() { return skeleton_num_joints(this); }
	public inline function get_numSoaJoints() { return skeleton_num_soa_joints(this); }

	static function skeleton_num_joints(skeleton: Skeleton): Int { return 0; }
	static function skeleton_num_soa_joints(skeleton: Skeleton): Int { return 0; }

	// Never allocated from Haxe, does not need padding
}

@:keep
@:hlNative("ozz")
@:struct class Model
{
	public function new() { model_new(this); }

	public inline function loadSkeleton(data: Bytes, len: Int) : Bool { return model_load_skeleton(this, data, len); }
	public inline function loadMeshes(data: Bytes, len: Int) : Bool { return model_load_meshes(this, data, len); }
	public inline function getSkeleton() : Skeleton { return model_get_skeleton(this); }
	public inline function getMeshes() : hl.NativeArray<Mesh> { return model_get_meshes(this); }
	public inline function getSkinMatrices( meshIndex: Int, numMatrices: Int ) : Bytes { return model_get_skin_matrices(this, meshIndex).toBytes( numMatrices * 4 * 4 ); }
	public inline function runSamplingJob( job: SamplingJob ) : Bool { return model_run_sampling_job(this, job); }

	static function model_new(model: Model): Void {}
	static function model_load_skeleton(model: Model, data: hl.Bytes, len: Int): Bool { return false; }
	static function model_load_meshes(model: Model, data: hl.Bytes, len: Int): Bool { return false; }
	static function model_get_skeleton(model: Model): Skeleton { return null; }
	static function model_get_meshes(model: Model): hl.NativeArray<Mesh> { return null; }
	static function model_get_skin_matrices(model: Model, meshIndex: Int): hl.Bytes { return null; }
	static function model_run_sampling_job(model: Model, job: SamplingJob): Bool { return false; }

	private var padding1: Float;
	private var padding2: Float;
	private var padding3: Float;
	private var padding4: Float;
	private var padding5: Float;
	private var padding6: Float;
	private var padding7: Float;
	private var padding8: Float;
	private var padding9: Float;
	private var padding10: Float;
	private var padding11: Float;
	private var padding12: Float;
	private var padding13: Float;
	private var padding14: Float;
	private var padding15: Float;
	private var padding16: Float;
	private var padding17: Float;
	private var padding18: Float;
	private var padding19: Float;
	private var padding20: Float;
	private var padding21: Float;
	private var padding22: Float;
	private var padding23: Float;
	private var padding24: Float;
	private var padding25: Float;
	private var padding26: Float;
	private var padding27: Float;
	private var padding28: Float;
	private var padding29: Float;
	private var padding30: Float;
	private var padding31: Float;
	private var padding32: Float;
	private var padding33: Float;
	private var padding34: Float;
	private var padding35: Float;
	private var padding36: Float;
	private var padding37: Float;
	private var padding38: Float;
	private var padding39: Float;
	private var padding40: Float;
	private var padding41: Float;
	private var padding42: Float;
	private var padding43: Float;
	private var padding44: Float;
	private var padding45: Float;
	private var padding46: Float;

}

class Init
{
	public static function init(onReady:Void -> Void) {
		onReady();
	}
}





#end
