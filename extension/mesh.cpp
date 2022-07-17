#include "utils.h"
#include "mesh.h"
#include "ozz/base/containers/vector_archive.h"
#include "ozz/base/io/archive.h"
#include "ozz/base/io/stream.h"
#include "ozz/base/memory/allocator.h"
#include "ozz/base/maths/math_archive.h"
#include "ozz/base/maths/simd_math_archive.h"



// @todo: Currently we don't share skeletons/meshes between instances. This is pretty bad!
#ifdef EMSCRIPTEN
int Mesh::load(std::string data, int len)
#else
int Mesh::load(char *data, int len)
#endif
{
	ozz::io::MemoryStream ms;
	#ifdef EMSCRIPTEN
	ms.Write( data.data(), len );
	#else
	ms.Write( data, len );
	#endif
	ms.Seek(0,ms.kSet);

	ozz::io::IArchive archive(&ms);

	if (!archive.TestTag<Mesh>())
	{
		printf("Failed to load mesh instance from file\n");
		return 0;
	}

	archive >> *this;

	return ms.Tell();
}

inline void writeFloat( char *dst, float src )
{
	for(int i = 0; i < 4; i++)
		dst[i] = ((char*)&src)[i];
}
#ifdef EMSCRIPTEN
emscripten::val Mesh::get_vertex_buffer( )
#else
char* Mesh::get_vertex_buffer( )
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

	if( vertexBuffer != nullptr )
		free( vertexBuffer );

	vertexBufferSize = stride * count;

	vertexBuffer = (char *)malloc( sizeof(char) * ( stride * count ) );

	char* buff = vertexBuffer;


	int idx = 0;


	for(int p=0; p<parts.size(); p++ )
	{
		Part part = parts[p];

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

			// @todo Inheriting Heaps' 255 bone limit. Should warn somewhere if the model execeeds it
			buff[start + 72] = (uint8_t)part.joint_indices[i * 4 + 0];
			buff[start + 73] = (uint8_t)part.joint_indices[i * 4 + 1];
			buff[start + 74] = (uint8_t)part.joint_indices[i * 4 + 2];
			buff[start + 75] = (uint8_t)part.joint_indices[i * 4 + 3];

		}
	}

#ifdef EMSCRIPTEN
	return emscripten::val(emscripten::typed_memory_view(vertexBufferSize, vertexBuffer));
#else
	return (char*)vertexBuffer;
#endif

}

#ifdef EMSCRIPTEN
emscripten::val Mesh::get_indices( )
#else
char* Mesh::get_indices( )
#endif
{
	int count = triangle_index_count();

	if( indexBuffer != nullptr )
		free( indexBuffer );

	indexBuffer = (uint16_t *)malloc( count * sizeof( uint16_t ) );

	for(size_t i=0; i<triangle_indices.size(); i++ )
	{
		indexBuffer[i] = triangle_indices[i];
	}


#ifdef EMSCRIPTEN
	return emscripten::val(emscripten::typed_memory_view(count, indexBuffer));
#else
	return (char *)indexBuffer;
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

		.function("loadImpl", &Mesh::load)
		.function("getVertexBufferImpl", &Mesh::get_vertex_buffer)
		.function("getIndices", &Mesh::get_indices)
		.function("loadMeshImpl", &Mesh::load)

		.property("triangleIndexCount", &Mesh::triangle_index_count)
		.property("vertexCount", &Mesh::vertex_count)
		.property("maxInfluencesCount", &Mesh::max_influences_count)
		.property("skinned", &Mesh::skinned)
		.property("jointCount", &Mesh::num_joints)
		.property("highestJointIndex", &Mesh::highest_joint_index)
		//.property("parts_count", &Mesh::parts_count)
		//.property("parts", &Mesh::parts)
		//.property("triangleIndices", &Mesh::triangle_indices)
		//.function("load", &Mesh::load)
		//.class_property("name", &animation_get_name)
    ;
}
#endif
#ifdef HL


static void mesh_finalize(hl_mesh *m)
{
	m->mesh.~Mesh();
}

/**
 * We use this to allocate the object with a finalizer; since Haxe doesn't have native
 * destructors, this is the only way to currently have this kind of behavior.
 */
HL_PRIM hl_mesh* HL_NAME(mesh_init)()
{
	hl_mesh* hl_mem = (hl_mesh*)hl_gc_alloc_finalizer(sizeof(hl_mesh));
	hl_mem->finalize = mesh_finalize;
	new (&hl_mem->mesh)Mesh();
	return hl_mem;
}

HL_PRIM vbyte* HL_NAME(mesh_get_vertex_buffer)(hl_mesh* m) {
	return (vbyte*)m->mesh.get_vertex_buffer();
}

HL_PRIM bool HL_NAME(mesh_load)(hl_mesh* m, vbyte* data, int length) {
	return m->mesh.load((char *)data, length);
}


HL_PRIM vbyte* HL_NAME(mesh_get_indices)(hl_mesh* m) {
	return (vbyte*)m->mesh.get_indices();
}

HL_PRIM int HL_NAME(mesh_triangle_index_count)(hl_mesh* m) {
	return m->mesh.triangle_index_count();
}

HL_PRIM int HL_NAME(mesh_vertex_count)(hl_mesh* m) {
	return m->mesh.vertex_count();
}

HL_PRIM int HL_NAME(mesh_max_influences_count)(hl_mesh* m) {
	return m->mesh.max_influences_count();
}

HL_PRIM bool HL_NAME(mesh_skinned)(hl_mesh* m) {
	return m->mesh.skinned();
}

HL_PRIM int HL_NAME(mesh_joint_count)(hl_mesh* m) {
	return m->mesh.num_joints();
}

HL_PRIM int HL_NAME(mesh_highest_joint_index)(hl_mesh* m) {
	return m->mesh.highest_joint_index();
}

HL_PRIM int HL_NAME(mesh_parts_count)(hl_mesh* m) {
	return m->mesh.parts_count();
}

DEFINE_PRIM(_STRUCT, mesh_init, _NO_ARG );
DEFINE_PRIM(_I32, mesh_load, _STRUCT _BYTES _I32);
DEFINE_PRIM(_BYTES, mesh_get_vertex_buffer, _STRUCT);
DEFINE_PRIM(_BYTES, mesh_get_indices, _STRUCT);
DEFINE_PRIM(_I32, mesh_triangle_index_count, _STRUCT);
DEFINE_PRIM(_I32, mesh_vertex_count, _STRUCT);
DEFINE_PRIM(_I32, mesh_max_influences_count, _STRUCT);
DEFINE_PRIM(_BOOL, mesh_skinned, _STRUCT);
DEFINE_PRIM(_I32, mesh_joint_count, _STRUCT);
DEFINE_PRIM(_I32, mesh_highest_joint_index, _STRUCT);
DEFINE_PRIM(_I32, mesh_parts_count, _STRUCT);


#endif