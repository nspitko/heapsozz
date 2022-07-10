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
import ozz.Native.Mesh;
import h3d.prim.HMDModel;

class OzzMesh extends MultiMaterial
{
	var skinShader : SkinBase;
	var model: ozz.Native.Model;
	var mesh: ozz.Native.Mesh;
	var meshIndex: Int;

	public function new( ozzModel: ozz.Native.Model, ozzMesh: ozz.Native.Mesh, meshIdx: Int, shader: SkinBase, mats: Array<Material>, ?parent: Object )
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

	override function draw( ctx : RenderContext )
	{
		// Assumption: The same model will be drawn in multiple passes sequentially
		// This MAY NOT BE TRUE if the engine tries to optimize for lower texture rebinds
		// please verify.
		if( ctx.drawPass.index == 0)
		{
			// @todo this sucks
			var skinMatrices = model.getSkinMatrices( meshIndex );

			var idx = 0;
			var midx = 0;
			while( idx < skinMatrices.length )
			{
				var matrix = skinShader.bonesMatrixes[midx++];
				//
				matrix._11 = skinMatrices[idx++];
				matrix._12 = skinMatrices[idx++];
				matrix._13 = skinMatrices[idx++];
				matrix._14 = skinMatrices[idx++];
				//
				matrix._21 = skinMatrices[idx++];
				matrix._22 = skinMatrices[idx++];
				matrix._23 = skinMatrices[idx++];
				matrix._24 = skinMatrices[idx++];
				//
				matrix._31 = skinMatrices[idx++];
				matrix._32 = skinMatrices[idx++];
				matrix._33 = skinMatrices[idx++];
				matrix._34 = skinMatrices[idx++];
				//
				matrix._41 = skinMatrices[idx++];
				matrix._42 = skinMatrices[idx++];
				matrix._43 = skinMatrices[idx++];
				matrix._44 = skinMatrices[idx++];

			}

			//trace('Applied ${midx} skin matrices this frame (of ${skinShader.bonesMatrixes.length} total matrices)');

		}

		ctx.uploadParams();


		super.draw(ctx);
	}

}