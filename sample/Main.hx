import h3d.mat.MaterialSetup;
import h3d.scene.Mesh;
import h3d.mat.Texture;

class Main extends hxd.App {

	#if !js
	var multiLoad: hl.NativeArray<ozz.Ozz.Mesh>;
	#end

	override function init()
	{

		new h3d.scene.fwd.DirLight(new h3d.Vector( 0.3, -0.4, -0.9), s3d);


		var model = new ozz.Ozz.Model();

		var bytes = hxd.Res.ozz_skin_mesh.entry.getBytes();
		model.loadMeshes( bytes, bytes.length );

		var bytes = hxd.Res.ozz_skin_skeleton.entry.getBytes();
		model.loadSkeleton( bytes, bytes.length );

		var skeleton = model.getSkeleton();

		trace('Skeleton has ${skeleton.numJoints} joints ( ${skeleton.numSoaJoints} SOA ) ');

		var ozzmodel = new ozz.OzzModel( model, s3d );


		var bytes = hxd.Res.ozz_skin_animation.entry.getBytes();
		var nativeAnimation = new ozz.Ozz.Animation();

		#if js
		trace('bytes len=${bytes.length}');
		ozz.Ozz.Animation.load( nativeAnimation, @:privateAccess bytes.b, bytes.length );
		#else
		nativeAnimation.load( bytes, bytes.length );
		#end

		#if js
		//trace('Animation name: ${ nativeAnimation.name().toString() }');
		#else
		//trace('Animation name: ${ @:privateAccess String.fromUTF8( nativeAnimation.name() ) }');
		#end
		trace('info: ${nativeAnimation.duration }s long, ${nativeAnimation.trackCount} tracks (${nativeAnimation.soaTrackCount} SOA)');


		ozzmodel.ozzAnim = nativeAnimation;
		new h3d.scene.fwd.DirLight(new h3d.Vector(1, 2, -4), s3d);


		new h3d.scene.CameraController(6, s3d);
	}

	override function update(dt:Float) {

	}

	static function main() {
		#if hl
		hxd.Res.initLocal();
		new Main();
		#else
		hxd.Res.initEmbed();
		ozz.Ozz.Init.init(() -> { new Main(); });
		#end

	}

}