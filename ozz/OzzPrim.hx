package ozz;
import js.lib.DataView;
import haxe.io.Bytes;
import haxe.io.BytesBuffer;
import h3d.Engine;
import hxd.IndexBuffer;
import h3d.col.Point;
import hxd.FloatBuffer;
import h3d.prim.MeshPrimitive;
import ozz.Native.Mesh;
import h3d.prim.HMDModel;

class OzzPrim extends MeshPrimitive {

	public var idx : hxd.IndexBuffer;
	var mesh: ozz.Native.Mesh;

	public function new( mesh: ozz.Native.Mesh = null) {
		this.mesh = mesh;

		trace('init prim. ${mesh.parts_count} parts with ${mesh.num_joints} joints.');

	}


	override function alloc( engine : h3d.Engine )
	{
		dispose();


		// @todo get_buffer currently doesn't handle strides other than the ones we specify here
		// Should be able to adapt!

		//var size = 23;
		var stride = 19;

		var names = ["position", "normal", "tangent", "uv", "color","weights","indexes"];
		var offsets = [0,3,6,9,11,15,18];
		var vertexCount = mesh.vertex_count;

		var rawBuffer = mesh.getVertexBuffer();

		#if js

		// This sucks
		var bytes = haxe.io.Bytes.alloc( rawBuffer.byteLength );
		for( i in 0 ... rawBuffer.byteLength )
		{
			bytes.set( i, rawBuffer[i]);
		}

		//var bytes = haxe.io.Bytes.ofData( rawBuffer.buffer );


		#else
		var bytes = rawBuffer.toBytes( vertexCount * stride );
		#end


		var flags : Array<h3d.Buffer.BufferFlag> = [];
		flags.push(Triangles);
		//if( normals == null || tangents != null ) flags.push(RawFormat);
		buffer = new h3d.Buffer(vertexCount, stride, flags );
		buffer.uploadBytes( bytes, 0, vertexCount );


		for( i in 0...names.length )
			addBuffer(names[i], buffer, offsets[i]);


		//addBuffer("indexes", bbuffer );

		// @todo copy this better
		var idxSrc = mesh.getIndices();
		idx = new IndexBuffer(mesh.triangle_index_count);

		for( i in 0 ... idxSrc.length )
		{

			idx.push(idxSrc[i]);
		}


		indexes = h3d.Indexes.alloc(idx);

	}

	/*
	override function getCollider() : h3d.col.Collider {
		var vertexes = new haxe.ds.Vector<hxd.impl.Float32>(points.length * 3);
		var indexes = new haxe.ds.Vector<Int>(idx.length);
		var vid = 0;
		for( p in points ) {
			vertexes[vid++] = p.x;
			vertexes[vid++] = p.y;
			vertexes[vid++] = p.z;
		}
		for( i in 0...idx.length )
			indexes[i] = idx[i];
		var poly = new h3d.col.Polygon();
		poly.addBuffers(vertexes, indexes);
		return poly;
	}
*/
	override function render( engine : h3d.Engine ) {
		if( buffer == null || buffer.isDisposed() )
			alloc(engine);
		var bufs = getBuffers(engine);
		if( indexes != null )
			engine.renderMultiBuffers(bufs, indexes);
		else if( buffer.flags.has(Quads) )
			engine.renderMultiBuffers(bufs, engine.mem.quadIndexes, 0, triCount());
		else
			engine.renderMultiBuffers(bufs, engine.mem.triIndexes, 0, triCount());
	}

}