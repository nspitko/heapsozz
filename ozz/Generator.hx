package ozz;

import sys.FileSystem;
#if eval
class Generator {

	static var INCLUDE = '
#ifdef _WIN32
#pragma warning(disable:4305)
#pragma warning(disable:4244)
#pragma warning(disable:4316)
#endif

#include "ozz/base/platform.h"
#include "ozz/base/span.h"
#include "ozz/animation/runtime/animation.h"
#include "ozz/animation/runtime/local_to_model_job.h"
#include "ozz/animation/runtime/sampling_job.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/animation/runtime/track.h"
#include "ozz/animation/runtime/track_sampling_job.h"
#include "ozz/animation/runtime/track_triggering_job.h"
#include "ozz/base/log.h"
#include "ozz/base/maths/box.h"
#include "ozz/base/maths/simd_math.h"
#include "ozz/base/maths/soa_transform.h"
#include "ozz/base/maths/vec_float.h"
#include "ozz/options/options.h"
#include "ozz/base/containers/string.h"
#include "ozz/base/memory/unique_ptr.h"
#include "ozz/base/containers/vector.h"


#include "utils.h"
#include "mesh.h"
#include "skeleton.h"
#include "animation.h"
#include "model.h"

';

	static var options = { idlFile : "ozz/ozz.idl", nativeLib : "ozz", outputDir : "extension/", includeCode : INCLUDE, autoGC : true };

	public static function generateCpp() {
		webidl.Generate.generateCpp(options);
	}

	static function recursiveFiles( path: String )
	{
		var out = [];
		var files = FileSystem.readDirectory(path);
		for( f in files )
		{
			if(  FileSystem.isDirectory( f ) )
				out = out.concat( recursiveFiles( f ) );
			else
			{
				if( StringTools.endsWith(f,".cc") )
					out.push( f );
			}
		}
		return out;
	}

	public static function getFiles() {

		return recursiveFiles("extension/ozz");
	}

	public static function generateJs() {
		// ammo.js params
		var debug = false;
		var defines = debug ? [] : ["NO_EXIT_RUNTIME=1", "NO_FILESYSTEM=1", "AGGRESSIVE_VARIABLE_ELIMINATION=1", "ELIMINATE_DUPLICATE_FUNCTIONS=1", "NO_DYNAMIC_EXECUTION=1"];
		var params = ["-O"+(debug?0:3), "--llvm-lto", "1", "-I", "../../include/bullet/src"];
		for( d in defines ) {
			params.push("-s");
			params.push(d);
		}
		webidl.Generate.generateJs(options, getFiles(), params);
	}

}
#end