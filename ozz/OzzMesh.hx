package ozz;
import h3d.scene.Object;
import h3d.shader.SkinBase;
import h3d.mat.Material;
import h3d.scene.RenderContext;
import h3d.scene.MultiMaterial;
import h3d.Engine;
import hxd.IndexBuffer;
import h3d.col.Point;
import hxd.FloatBuffer;
import h3d.prim.MeshPrimitive;
import ozz.Ozz.Mesh;
import h3d.prim.HMDModel;

class OzzMesh extends MultiMaterial
{
	var skinShader : SkinBase;
	var model: ozz.Ozz.Model;
	var mesh: ozz.Ozz.Mesh;
	var meshIndex: Int;

	public function new( ozzModel: ozz.Ozz.Model, ozzMesh: ozz.Ozz.Mesh, meshIdx: Int, shader: SkinBase, mats: Array<Material>, ?parent: Object )
	{
		meshIndex = meshIdx;
		skinShader = shader;
		model = ozzModel;
		mesh = ozzMesh;
		var prim = new OzzPrim( mesh );
		super(prim, mats, parent );
		// SDSDSDDS
		// Remember: One emit -> one draw
		// need to split out parts per material to get proper skinning
		// but only bind matrices once per mesh thankfully
	}

	#if hl
	inline function getMatrix( bytes: hl.Bytes, idx: Int ): Float
	{
		return bytes.getF32(idx * 4);
	}
	#else
	inline function getMatrix( bytes: js.lib.Float32Array, idx: Int ): Float
	{
		return bytes[idx];
	}
	#end


	override function draw( ctx : RenderContext )
	{
		// Assumption: The same model will be drawn in multiple passes sequentially
		// This MAY NOT BE TRUE if the engine tries to optimize for lower texture rebinds
		// please verify.
		if( ctx.drawPass.index == 0)
		{
			// @todo this sucks
			var numMatrices = 0;
			var skinMatrices = model.getSkinMatrices( meshIndex, mesh.jointCount );

			var idx = 0;
			var midx = 0;


			while( idx < skinMatrices.length )
			{
				var matrix = skinShader.bonesMatrixes[midx++];

				//
				matrix._11 = getMatrix( skinMatrices, idx++ );
				matrix._12 = getMatrix( skinMatrices, idx++ );
				matrix._13 = getMatrix( skinMatrices, idx++ );
				matrix._14 = getMatrix( skinMatrices, idx++ );
				//
				matrix._21 = getMatrix( skinMatrices, idx++ );
				matrix._22 = getMatrix( skinMatrices, idx++ );
				matrix._23 = getMatrix( skinMatrices, idx++ );
				matrix._24 = getMatrix( skinMatrices, idx++ );
				//
				matrix._31 = getMatrix( skinMatrices, idx++ );
				matrix._32 = getMatrix( skinMatrices, idx++ );
				matrix._33 = getMatrix( skinMatrices, idx++ );
				matrix._34 = getMatrix( skinMatrices, idx++ );
				//
				matrix._41 = getMatrix( skinMatrices, idx++ );
				matrix._42 = getMatrix( skinMatrices, idx++ );
				matrix._43 = getMatrix( skinMatrices, idx++ );
				matrix._44 = getMatrix( skinMatrices, idx++ );

			}



			//trace('Applied ${midx} skin matrices this frame (of ${skinShader.bonesMatrixes.length} total matrices)');

		}

		ctx.uploadParams();


		super.draw(ctx);
	}

}