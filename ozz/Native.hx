package ozz;


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

	public function getVertexBuffer() : js.lib.Uint8Array;
	public function getIndices() : js.lib.Uint16Array;

	public var triangle_index_count : Int;
	public var vertex_count : Int;
	public var max_influences_count : Int;
	public var skinned : Bool;
	public var num_joints : Int;
	public var highest_joint_index : Int;

	public var triangle_indices : Array<Int>;
	public var parts : Array<Part>;
	public var parts_count : Int;




}


@:native("ozz.Skeleton")
extern class Skeleton {

	public var num_joints : Int;
	public var num_soa_joints : Int;
	public var joint_parents : Array<Int>;

}

@:native("ozz.Model")
extern class Model {
	public function new(): Void;
	public function loadMeshes( data: js.lib.Uint8Array, len: Int ): Bool;
	public function loadSkeleton( data: js.lib.Uint8Array, len: Int ): Bool;
	public function getSkeleton(): Skeleton;
	public function getMeshes(): VectorMeshPtr;
	public function getSkinMatrices( modelIndex: Int ) : js.lib.Float32Array;
	public function runSamplingJob( samplingJob: SamplingJob ): Bool;
	//
	public var duration : Float;
	public var trackCount : Int;
	public var soaTrackCount : Int;

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

abstract SamplingJob(webidl.Types.Ref) to webidl.Types.Ref
{
        @:hlNative("ozz", "SamplingJob_new0")
        static function new0():SamplingJob return null;
        public inline function new():Void this = new0();
        @:hlNative("ozz", "SamplingJob_validate0")
        public function validate():Bool return false;
        public var ratio(get, set) : Single;
        @:hlNative("ozz", "SamplingJob_get_ratio")
        function get_ratio():Single return 0.;
        @:hlNative("ozz", "SamplingJob_set_ratio")
        function set_ratio(_v:Single):Single return 0.;
        @:hlNative("ozz", "SamplingJob_setAnimation1")
        public function setAnimation(anim:Animation):Void { }
        @:hlNative("ozz", "SamplingJob_delete")
        public function delete():Void { }
}

abstract Part(webidl.Types.Ref) to webidl.Types.Ref
{
        @:hlNative("ozz", "Part_new0")
        static function new0():Part return null;
        public inline function new():Void this = new0();
        @:hlNative("ozz", "Part_get_positions")
        public function getpositions(index:Int):Single return 0.;
        @:hlNative("ozz", "Part_set_positions")
        public function setpositions(index:Int, _v:Single):Single return 0.;
        @:hlNative("ozz", "Part_positions_count0")
        public function positions_count():Int return 0;
        @:hlNative("ozz", "Part_vertex_count0")
        public function vertex_count():Int return 0;
        @:hlNative("ozz", "Part_influencesCount0")
        public function influencesCount():Int return 0;
        @:hlNative("ozz", "Part_delete")
        public function delete():Void { }
}

abstract Mesh(webidl.Types.Ref) to webidl.Types.Ref
{
        @:hlNative("ozz", "Mesh_new0")
        static function new0():Mesh return null;
        public inline function new():Void this = new0();
        @:hlNative("ozz", "Mesh_triangle_index_count0")
        public function triangle_index_count():Int return 0;
        @:hlNative("ozz", "Mesh_vertex_count0")
        public function vertex_count():Int return 0;
        @:hlNative("ozz", "Mesh_max_influences_count0")
        public function max_influences_count():Int return 0;
        @:hlNative("ozz", "Mesh_skinned0")
        public function skinned():Bool return false;
        @:hlNative("ozz", "Mesh_num_joints0")
        public function num_joints():Int return 0;
        @:hlNative("ozz", "Mesh_get_triangle_indices")
        public function gettriangle_indices(index:Int):hl.UI16 return 0;
        @:hlNative("ozz", "Mesh_set_triangle_indices")
        public function settriangle_indices(index:Int, _v:hl.UI16):hl.UI16 return 0;
        @:hlNative("ozz", "Mesh_get_JointRemap")
        public function getJointRemap(index:Int):hl.UI16 return 0;
        @:hlNative("ozz", "Mesh_set_JointRemap")
        public function setJointRemap(index:Int, _v:hl.UI16):hl.UI16 return 0;
        @:hlNative("ozz", "Mesh_jointRemapCount0")
        public function jointRemapCount():Int return 0;
        @:hlNative("ozz", "Mesh_highestJointIndex0")
        public function highestJointIndex():Int return 0;
        @:hlNative("ozz", "Mesh_load2")
        public function load(data:hl.Bytes, length:Int):Void { }
        @:hlNative("ozz", "Mesh_parts_count0")
        public function parts_count():Int return 0;
        @:hlNative("ozz", "Mesh_get_indices0")
        public function get_indices():hl.NativeArray<UInt> return null;
        @:hlNative("ozz", "Mesh_getRawBuffer0")
        public function getRawBuffer():hl.Bytes return null;
        @:hlNative("ozz", "Mesh_delete")
        public function delete():Void { }
}

abstract Skeleton(webidl.Types.Ref) to webidl.Types.Ref
{
        @:hlNative("ozz", "Skeleton_new0")
        static function new0():Skeleton return null;
        public inline function new():Void this = new0();
        @:hlNative("ozz", "Skeleton_load2")
        public function load(data:hl.Bytes, length:Int):Void { }
        @:hlNative("ozz", "Skeleton_jointCount0")
        public function jointCount():Int return 0;
        @:hlNative("ozz", "Skeleton_soaJointCount0")
        public function soaJointCount():Int return 0;
        @:hlNative("ozz", "Skeleton_jointNameCount0")
        public function jointNameCount():Int return 0;
        @:hlNative("ozz", "Skeleton_getJointName1")
        public function getJointName(idx:Int):hl.Bytes return null;
        @:hlNative("ozz", "Skeleton_delete")
        public function delete():Void { }
}

abstract Animation(webidl.Types.Ref) to webidl.Types.Ref
{
        @:hlNative("ozz", "Animation_new0")
        static function new0():Animation return null;
        public inline function new():Void this = new0();
        @:hlNative("ozz", "Animation_load2")
        public function load(data:hl.Bytes, length:Int):Void { }
        @:hlNative("ozz", "Animation_trackCount0")
        public function trackCount():Int return 0;
        @:hlNative("ozz", "Animation_soaTrackCount0")
        public function soaTrackCount():Int return 0;
        @:hlNative("ozz", "Animation_name0")
        public function name():hl.Bytes return null;
        @:hlNative("ozz", "Animation_duration0")
        public function duration():Single return 0.;
        @:hlNative("ozz", "Animation_delete")
        public function delete():Void { }
}

abstract Model(webidl.Types.Ref) to webidl.Types.Ref
{
        @:hlNative("ozz", "Model_new0")
        static function new0():Model return null;
        public inline function new():Void this = new0();
        @:hlNative("ozz", "Model_loadMeshes2")
        public function loadMeshes(data:hl.Bytes, length:Int):Bool return false;
        @:hlNative("ozz", "Model_loadSkeleton2")
        public function loadSkeleton(data:hl.Bytes, length:Int):Bool return false;
        @:hlNative("ozz", "Model_runSamplingJob1")
        public function runSamplingJob(job:SamplingJob):Bool return false;
        @:hlNative("ozz", "Model_getSkinMatrices1")
        public function getSkinMatrices(meshIndex:Int):hl.NativeArray<Single> return null;
        @:hlNative("ozz", "Model_getMeshes0")
        public function getMeshes():hl.NativeArray<Mesh> return null;
        @:hlNative("ozz", "Model_getSkeleton0")
        public function getSkeleton():Skeleton return null;
        @:hlNative("ozz", "Model_delete")
        public function delete():Void { }
}
class Init {
        public static function init(onReady:Void -> Void) {
                onReady();
        }
}



#end
