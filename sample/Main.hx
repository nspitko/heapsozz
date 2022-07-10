import h3d.mat.MaterialSetup;
import h3d.scene.Mesh;
import h3d.mat.Texture;

class Main extends hxd.App {

	var multiLoad: hl.NativeArray<ozz.Native.Mesh>;

	override function init()
	{

		new h3d.scene.fwd.DirLight(new h3d.Vector( 0.3, -0.4, -0.9), s3d);

		var model = new ozz.Native.Model();

		var bytes = hxd.Res.ozz_skin_mesh.entry.getBytes();
		model.loadMeshes( bytes, bytes.length );

		var bytes = hxd.Res.ozz_skin_skeleton.entry.getBytes();
		model.loadSkeleton( bytes, bytes.length );

		var ozzmodel = new ozz.OzzModel( model, s3d );



		var bytes = hxd.Res.ozz_skin_animation.entry.getBytes();
		var nativeAnimation = new ozz.Native.Animation();
		nativeAnimation.load( bytes, bytes.length );

		trace('Animation name: ${ @:privateAccess String.fromUTF8( nativeAnimation.name() ) }');
		trace('info: ${nativeAnimation.duration() }s long, ${nativeAnimation.trackCount()} tracks (${nativeAnimation.soaTrackCount()} SOA)');

		ozzmodel.ozzAnim = nativeAnimation;


		new h3d.scene.fwd.DirLight(new h3d.Vector(1, 2, -4), s3d);


		new h3d.scene.CameraController(6, s3d);
	}

	override function update(dt:Float) {

	}

	static function main() {
		hxd.Res.initLocal();
		new Main();
	}

}