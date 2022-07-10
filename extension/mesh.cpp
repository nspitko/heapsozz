#include <hl.h>
#include "utils.h"
#include "mesh.h"
#include "ozz/base/containers/vector_archive.h"
#include "ozz/base/io/archive.h"
#include "ozz/base/io/stream.h"
#include "ozz/base/memory/allocator.h"
#include "ozz/base/maths/math_archive.h"
#include "ozz/base/maths/simd_math_archive.h"

//static void finalize_Mesh( _ref(Mesh)* _this ) { free_ref(_this ); }

inline void writeFloat( vbyte *dst, float src )
{
	for(int i = 0; i < 4; i++)
		dst[i] = ((char*)&src)[i];
}

vbyte* Mesh::getRawBuffer( )
{
	// @todo we should just pass the buffer in from haxe and write straight to it.

	int count = vertex_count();

	// position:vec3 normal:vec3 tangent:vec3 uv:vec2 color:vec4 weights:bytes4

	int fWidth = 4;
	int byteWidth = 1;

	int stride =	fWidth * 3 + // pos
					fWidth * 3 + // norm
					fWidth * 3 + // tan
					fWidth * 2 + // uv
					fWidth * 4 + // color
					fWidth * 3 + // weight
					byteWidth * 4; // idx

	vbyte* buff = hl_alloc_bytes( stride * count );

	int idx = 0;


	for(int p=0; p<parts.size(); p++ )
	{
		Part part = parts[p];

		printf("part influences count=%d, weights=%d, indices = %d\n", part.influences_count(), part.joint_weights.size(), part.joint_indices.size());

		int indexStride = part.influences_count();
		int weightStride = indexStride - 1;

		// @todo Can't currnetly handle other strides.
		assert( indexStride == 4 );


		int posOffset = 0;
		int normalOffset = 0;
		int tanOffset = 0;
		int uvOffset = 0;
		int colorOffset = 0;
		int indexOffset = 0;
		int weightOffset = 0;

		printf("Writing buffer for part %d from idx %d\n", p, idx);


		for( int i=0; i< count; i++ )
		{
			int start = i * stride;

			writeFloat(buff + start + 0, part.positions[i * 3 + 0] );
			writeFloat(buff + start + 4, part.positions[i * 3 + 1] );
			writeFloat(buff + start + 8, part.positions[i * 3 + 2] );

			writeFloat(buff + start + 12, part.normals[i * 3 + 0] );
			writeFloat(buff + start + 16, part.normals[i * 3 + 1] );
			writeFloat(buff + start + 20, part.normals[i * 3 + 2] );

			writeFloat(buff + start + 24, part.tangents[i * 3 + 0] );
			writeFloat(buff + start + 28, part.tangents[i * 3 + 1] );
			writeFloat(buff + start + 32, part.tangents[i * 3 + 2] );

			writeFloat(buff + start + 36, part.uvs[i * 2 + 0] );
			writeFloat(buff + start + 40, part.uvs[i * 2 + 1] );

			// !! Hack !!
			// Not all models have colors, specify them anyway.
			// @todo figure out a way to specify buffer stride/settings properly
			if( part.colors.size() > 0 )
			{
				writeFloat(buff + start + 44, part.colors[i * 4 + 0] );
				writeFloat(buff + start + 48, part.colors[i * 4 + 1] );
				writeFloat(buff + start + 52, part.colors[i * 4 + 2] );
				writeFloat(buff + start + 56, part.colors[i * 4 + 3] );
			}
			else
			{
				writeFloat(buff + start + 44, 1.0f );
				writeFloat(buff + start + 48, 1.0f );
				writeFloat(buff + start + 52, 1.0f );
				writeFloat(buff + start + 56, 1.0f );
			}

			// weights

			writeFloat(buff + start + 60, part.joint_weights[i * 3 + 0] );
			writeFloat(buff + start + 64, part.joint_weights[i * 3 + 1] );
			writeFloat(buff + start + 68, part.joint_weights[i * 3 + 2] );

			buff[start + 72] = part.joint_indices[i * 4 + 0];
			buff[start + 73] = part.joint_indices[i * 4 + 1];
			buff[start + 74] = part.joint_indices[i * 4 + 2];
			buff[start + 75] = part.joint_indices[i * 4 + 3];

		}
	}

	return buff;

}

bool Mesh::load(vbyte* data, int len)
{
	printf("Loading mesh archive\n" );


	ozz::io::MemoryStream ms;
	ms.Write( data, len );
	ms.Seek(0,ms.kSet);

	ozz::io::IArchive archive(&ms);


	if (!archive.TestTag<Mesh>())
	{
		printf("Failed to load mesh instance from file\n");
		return false;
	}

	// Once the tag is validated, reading cannot fail.
	archive >> *this;

	return true;
}

static uint32_t pack_u32(uint8_t x, uint8_t y, uint8_t z, uint8_t w) {
    return (uint32_t)(((uint32_t)w<<24)|((uint32_t)z<<16)|((uint32_t)y<<8)|x);
}

varray* Mesh::get_buffer( )
{
	// @todo we should just pass the buffer in from haxe and write straight to it.

	int count = 0;
	for(int i=0; i<parts.size(); i++ )
	{
		count += parts[i].positions.size();
		/*
		count += parts[i].normals.size();
		count += parts[i].tangents.size();
		count += parts[i].uvs.size();
		count += parts[i].colors.size();
		*/

	}

	// @todo: Size buffer based on actual data
	count = (count / 3) * 22;

	varray* vbuff = hl_alloc_array( &hlt_f32, count );
	float* buff = hl_aptr(vbuff, float);
	int idx = 0;


	for(int p=0; p<parts.size(); p++ )
	{
		Part part = parts[p];

		printf("part influences count=%d, weights=%d, indices = %d\n", part.influences_count(), part.joint_weights.size(), part.joint_indices.size());

		int indexStride = part.influences_count();
		int weightStride = indexStride - 1;

		// @todo Can't currnetly handle other strides.
		assert( indexStride == 4 );


		int posOffset = 0;
		int normalOffset = 0;
		int tanOffset = 0;
		int uvOffset = 0;
		int colorOffset = 0;
		int indexOffset = 0;
		int weightOffset = 0;

		printf("Writing buffer for part %d from idx %d\n", p, idx);


		for( int i=0; i<part.positions.size() / 3; i++ )
		{
			buff[idx++] = part.positions[posOffset++];
			buff[idx++] = part.positions[posOffset++];
			buff[idx++] = part.positions[posOffset++];

			buff[idx++] = part.normals[normalOffset++];
			buff[idx++] = part.normals[normalOffset++];
			buff[idx++] = part.normals[normalOffset++];

			buff[idx++] = part.tangents[tanOffset++];
			buff[idx++] = part.tangents[tanOffset++];
			buff[idx++] = part.tangents[tanOffset++];

			buff[idx++] = part.uvs[uvOffset++];
			buff[idx++] = part.uvs[uvOffset++];

			// !! Hack !!
			// Not all models have colors, specify them anyway.
			// @todo figure out a way to specify buffer stride/settings properly
			if (part.colors.size() > 0)
			{
				buff[idx++] = part.colors[colorOffset++];
				buff[idx++] = part.colors[colorOffset++];
				buff[idx++] = part.colors[colorOffset++];
				buff[idx++] = part.colors[colorOffset++];
			}
			else
			{
				buff[idx++] = 1.0f;
				buff[idx++] = 1.0f;
				buff[idx++] = 1.0f;
				buff[idx++] = 1.0f;
			}

			// weights

			buff[idx++] = part.joint_weights[weightOffset++];
			buff[idx++] = part.joint_weights[weightOffset++];
			buff[idx++] = part.joint_weights[weightOffset++];

			// indices
/*
 			const uint8_t ji0 = (uint8_t) part.joint_indices[indexOffset++];
            const uint8_t ji1 = (uint8_t) part.joint_indices[indexOffset++];
            const uint8_t ji2 = (uint8_t) part.joint_indices[indexOffset++];
            const uint8_t ji3 = (uint8_t) part.joint_indices[indexOffset++];
            buff[idx++] = pack_u32(ji0, ji1, ji2, ji3);
*/

			buff[idx++] = part.joint_indices[indexOffset++];
			buff[idx++] = part.joint_indices[indexOffset++];
			buff[idx++] = part.joint_indices[indexOffset++];
			buff[idx++] = part.joint_indices[indexOffset++];


			//
		}

		printf("Wrote %d weights and %d indices\n", weightOffset, indexOffset);
	}

	return vbuff;




	int buffidx = 0;
	int offset = 0;
	// positions
	for(int i=0; i<parts.size(); i++ )
	{
		int len = parts[i].positions.size();
		memcpy(buff, parts[i].positions.data(), len );
		buff += len;
	}

	// normals
	for(int i=0; i<parts.size(); i++ )
	{
		int len = parts[i].normals.size();
		memcpy(buff, parts[i].normals.data(), len );
		buff += len;
	}

	// Tangents
	for(int i=0; i<parts.size(); i++ )
	{
		int len = parts[i].tangents.size();
		memcpy(buff, parts[i].tangents.data(), len );
		buff += len;
	}

	// uvs
	for(int i=0; i<parts.size(); i++ )
	{
		int len = parts[i].uvs.size();
		memcpy(buff, parts[i].uvs.data(), len );
		buff += len;
	}

	// colors
	for(int i=0; i<parts.size(); i++ )
	{
		int len = parts[i].colors.size();
		memcpy(buff, parts[i].colors.data(), len );
		buff += len;
	}



	return vbuff;

}

varray* Mesh::get_indices( )
{
	int count = triangle_index_count();

	printf("Writing index buffer for %d verts\n", count);

	varray* buff = hl_alloc_array( &hlt_i32, count );
	uint32_t *indices = hl_aptr(buff, uint32_t);
	int idx = 0;

	for(int i=0; i<triangle_indices.size(); i++ )
	{
		indices[idx++] = triangle_indices[i];
	}




	return buff;
}


// Mesh loaders
void ozz::io::Extern<Part>::Save(ozz::io::OArchive& _archive,
																			const Part* _parts,
																			size_t _count) {
	for (size_t i = 0; i < _count; ++i) {
		const Part& part = _parts[i];
		_archive << part.positions;
		_archive << part.normals;
		_archive << part.tangents;
		_archive << part.uvs;
		_archive << part.colors;
		_archive << part.joint_indices;
		_archive << part.joint_weights;
	}
}

void ozz::io::Extern<Part>::Load(ozz::io::IArchive& _archive,
																			Part* _parts, size_t _count,
																			uint32_t _version) {
	(void)_version;
	for (size_t i = 0; i < _count; ++i) {
		Part& part = _parts[i];
		_archive >> part.positions;
		_archive >> part.normals;
		_archive >> part.tangents;
		_archive >> part.uvs;
		_archive >> part.colors;
		_archive >> part.joint_indices;
		_archive >> part.joint_weights;
	}
}

void ozz::io::Extern<Mesh>::Save(ozz::io::OArchive& _archive, const Mesh* _meshes,
																size_t _count) {
	for (size_t i = 0; i < _count; ++i) {
		const Mesh& mesh = _meshes[i];
		_archive << mesh.parts;
		_archive << mesh.triangle_indices;
		_archive << mesh.joint_remaps;
		_archive << mesh.inverse_bind_poses;
	}
}

void ozz::io::Extern<Mesh>::Load(ozz::io::IArchive& _archive, Mesh* _meshes,
																size_t _count, uint32_t _version) {
	(void)_version;
	for (size_t i = 0; i < _count; ++i) {
		Mesh& mesh = _meshes[i];
		_archive >> mesh.parts;
		_archive >> mesh.triangle_indices;
		_archive >> mesh.joint_remaps;
		_archive >> mesh.inverse_bind_poses;
	}
}
