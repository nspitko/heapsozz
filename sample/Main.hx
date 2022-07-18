import ozz.OzzModel;
import ozz.OzzLibrary;
import h3d.mat.MaterialSetup;
import h3d.scene.Mesh;
import h3d.mat.Texture;
import ozz.Ozz;

class Main extends hxd.App {

	#if !js
	var multiLoad: hl.NativeArray<ozz.Ozz.Mesh>;
	#end

	var library = new OzzLibrary();

	override function init()
	{

		new h3d.scene.fwd.DirLight(new h3d.Vector( 0.3, -0.4, -0.9), s3d);
		new h3d.scene.fwd.DirLight(new h3d.Vector(1, 2, -4), s3d);

		var modelData = library.loadModel( hxd.Res.ozz_skin_mesh, hxd.Res.ozz_skin_skeleton );
		var animation = library.loadAnimation(hxd.Res.ozz_skin_animation);

		var model = new OzzModel(modelData, s3d);
		model.playOzzAnimation( animation );


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