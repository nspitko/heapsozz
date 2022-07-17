package ozz;
import h3d.Matrix;
import h3d.scene.RenderContext;
import h3d.mat.Material;
import h3d.mat.Texture;
import h3d.scene.Object;
import h3d.Engine;
import hxd.IndexBuffer;
import h3d.col.Point;
import hxd.FloatBuffer;
import h3d.prim.MeshPrimitive;
import ozz.Ozz.Mesh;
import h3d.prim.HMDModel;

class OzzModel extends h3d.scene.Object {

	public var idx : hxd.IndexBuffer;
	var model: ozz.Ozz.Model;

	public var meshes: Array<OzzMesh> = [];
	public var heapsMeshes: Array<OzzMesh> = [];

	var materials: Array<Material> = [];

	var skinShader : h3d.shader.SkinBase;

	var skinMatrices : Array<h3d.Matrix> = [];

	// @todo
	public var ozzAnim: ozz.Ozz.Animation;
	var animRatio: Float = 0;

	public function new( ozz: ozz.Ozz.Model, ?parent: Object ) {

		super(parent);
		this.model = ozz;

		initialize();

	}

	function initialize()
	{

		var ozzMeshes = model.meshes;




		// @todo
		var hasNormalMap = false;


		var ozzShader = new ozz.OzzSkinShader();
		//ozzShader.stride = 4;
		skinShader = ozzShader; // hasNormalMap ? new h3d.shader.SkinTangent() : new h3d.shader.Skin();
		skinShader.fourBonesByVertex = true;


		// @todo
		var mat = h3d.mat.Material.create( Texture.fromColor(0x555555) );
		mat.shadows = false; // @todo needs bounds calcs
		materials = [mat];



		var maxBones: Int = 0;// model.getSkeleton().highestJointIndex();
		for( i in 0 ... ozzMeshes.length )
		{
			#if js
			maxBones = cast Math.max( maxBones, ozzMeshes.get(i).highestJointIndex );
			#else
			maxBones = cast Math.max( maxBones, ozzMeshes[i].highestJointIndex );
			#end
		}

		for( i in 0 ... maxBones )
		{
			skinMatrices.push( h3d.Matrix.I() );
			skinShader.bonesMatrixes = skinMatrices;
		}

		skinShader.MaxBones = cast Math.min( maxBones, 255 );

		for( m in materials )
		{

			if( m != null )
			{
				if( m.normalMap != null )
					@:privateAccess m.mainPass.addShaderAtIndex(skinShader, m.mainPass.getShaderIndex(m.normalShader) + 1);
				else
					m.mainPass.addShader(skinShader);

				//m.mainPass.dynamicParameters = true;

			}
		}


		for( i in 0 ... ozzMeshes.length )
		{
			#if js
			var mesh = new OzzMesh( model, ozzMeshes.get(i), i, skinShader, materials, this );
			#else
			var mesh = new OzzMesh( model, ozzMeshes[i], i, skinShader, materials, this );
			#end
			meshes.push( mesh );

		}



	}

	override function sync( ctx : RenderContext )
	{
		var job = new ozz.Ozz.SamplingJob();
		job.setAnimation( ozzAnim );
		job.ratio = animRatio;

	if( !model.runSamplingJob( job ) )
			trace("Sampling job failed");

		//skinShader.fourBonesByVertex = true; // @todo


		animRatio += 0.003;
		if(animRatio > 1 )
			animRatio = 0;
	}


}