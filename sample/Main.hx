import h3d.mat.MaterialSetup;
import h3d.scene.Mesh;
import h3d.mat.Texture;
import ozz.Ozz;

class Main extends hxd.App {

	#if !js
	var multiLoad: hl.NativeArray<ozz.Ozz.Mesh>;
	#end

	override function init()
	{

		new h3d.scene.fwd.DirLight(new h3d.Vector( 0.3, -0.4, -0.9), s3d);


		var model = new ozz.Ozz.Model();

		var bytes = hxd.Res.ozz_skin_mesh.entry.getBytes();
		var window = bytes;
		var pos = 0;
		var meshes = [];
		while( pos < bytes.length )
		{
			var mesh = new ozz.Ozz.Mesh();
			pos = mesh.load(window, bytes.length);
			trace(pos);
			if( pos == 0 )
				break;
			meshes.push( mesh );
			window = bytes.sub(pos, bytes.length - pos);
		}

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
		trace(tmpArray.length);
		trace(meshes.length);
		trace(model.meshes.length);
		#end

		var skeleton = new Skeleton();

		var bytes = hxd.Res.ozz_skin_skeleton.entry.getBytes();
		skeleton.load( bytes, bytes.length );

		model.skeleton = skeleton;

		trace('Skeleton has ${skeleton.numJoints} joints ( ${skeleton.numSoaJoints} SOA ) ');

		var ozzmodel = new ozz.OzzModel( model, s3d );


		var bytes = hxd.Res.ozz_skin_animation.entry.getBytes();
		var nativeAnimation = new ozz.Ozz.Animation();

		nativeAnimation.load( bytes, bytes.length );

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