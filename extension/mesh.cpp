#include "utils.h"
#include "mesh.h"
#include "ozz/base/containers/vector_archive.h"
#include "ozz/base/io/archive.h"
#include "ozz/base/io/stream.h"
#include "ozz/base/memory/allocator.h"
#include "ozz/base/maths/math_archive.h"
#include "ozz/base/maths/simd_math_archive.h"

inline void writeFloat( char *dst, float src )
{
	for(int i = 0; i < 4; i++)
		dst[i] = ((char*)&src)[i];
}
#ifdef EMSCRIPTEN
emscripten::val Mesh::getVertexBuffer( )
#else
vbyte* Mesh::getRawBuffer( )
#endif
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

#ifdef EMSCRIPTEN
	if( m_pVertexBuffer == nullptr )
		m_unVertexBufferSize = sizeof(char) * ( stride * count );

	m_pVertexBuffer = (char *)malloc( m_unVertexBufferSize );
	char* buff = m_pVertexBuffer;
#else
	vbyte* buff = hl_alloc_bytes( stride * count );
#endif


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

#ifdef EMSCRIPTEN
	return emscripten::val(emscripten::typed_memory_view(m_unVertexBufferSize, m_pVertexBuffer));
#else
	return buff;
#endif

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
#ifdef EMSCRIPTEN
emscripten::val Mesh::getIndices( )
#else
vbyte* Mesh::getIndices( )
#endif
{

	int count = triangle_index_count();

	printf("Writing index buffer for %d verts\n", count);

#ifdef EMSCRIPTEN
	if( m_pIndexBuffer == nullptr )
		m_pIndexBuffer = (uint16_t *)malloc( count * sizeof( uint16_t ) );

	for(int i=0; i<triangle_indices.size(); i++ )
	{
		m_pIndexBuffer[i] = triangle_indices[i];
	}

	return emscripten::val(emscripten::typed_memory_view(count, m_pIndexBuffer));

#else
	varray* buff = hl_alloc_array( &hlt_i32, count );
	uint32_t *indices = hl_aptr(buff, uint32_t);
	int idx = 0;

	for(int i=0; i<triangle_indices.size(); i++ )
	{
		indices[idx++] = triangle_indices[i];
	}





	return buff;
#endif



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


#ifdef EMSCRIPTEN

using namespace emscripten;

EMSCRIPTEN_BINDINGS(ozzMesh) {
	class_<Mesh>("Mesh")
		.constructor<>()

		.function("getVertexBuffer", &Mesh::getVertexBuffer)
		.function("getIndices", &Mesh::getIndices)

		.property("triangle_index_count", &Mesh::triangle_index_count)
		.property("vertex_count", &Mesh::vertex_count)
		.property("max_influences_count", &Mesh::max_influences_count)
		.property("skinned", &Mesh::skinned)
		.property("num_joints", &Mesh::num_joints)
		.property("highest_joint_index", &Mesh::highest_joint_index)
		//.property("parts_count", &Mesh::parts_count)
		//.property("parts", &Mesh::parts)
		.property("triangle_indices", &Mesh::triangle_indices)
		//.function("load", &Mesh::load)
		//.class_property("name", &animation_get_name)
    ;
}

#endif