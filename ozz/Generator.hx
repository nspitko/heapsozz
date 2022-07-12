package ozz;

import haxe.io.Path;
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

#include "mesh.h"
#include "skeleton.h"
#include "animation.h"
#include "model.h"

';

	static var opts = { nativeLib : "ozz", outputDir : "build", includeCode : INCLUDE, autoGC : true };

	static var cwd: String;
	static function relPath(path:String):String
	{
		if( cwd == null )
		{
			// Find the cwd... it's null on macro for some reason so lets find it the hard way
			var p = Path.directory(path);
			while( p.length > 2 && !FileSystem.exists( Path.join([p, "ozz.hdll.vcxproj.filters"]) ) )
			{
				var pa = p.split("/");
				pa.pop();
				p = pa.join("/");
			}
			cwd = p;
		}
		return StringTools.replace(path,'$cwd/',"");
    }



	public static function getFiles() {
		var prj = new haxe.xml.Access(Xml.parse(sys.io.File.getContent("ozz.hdll.vcxproj.filters")).firstElement());
		var sources = [];
		for( i in prj.elements )
			if( i.name == "ItemGroup" )
				for( f in i.elements ) {
					if( f.name != "ClCompile" ) continue;
					var fname = f.att.Include.split("\\").join("/");
					sources.push( relPath( fname ) );
				}
		return sources;
	}

	public static function generateJs() {
		var debug = true;
		var defines = debug ? [] : ["EMCC_DEBUG=1", "NO_FILESYSTEM=1", "AGGRESSIVE_VARIABLE_ELIMINATION=1", "ELIMINATE_DUPLICATE_FUNCTIONS=1", "NO_DYNAMIC_EXECUTION=1"];
		var params = ["-O"+(debug?0:3),"-I", "extension/ozz/include/","-I", "extension/ozz/src/", "-I", "c:\\haxetoolkit\\hl\\include"];
		for( d in defines ) {
			params.push("-s");
			params.push(d);
		}

		var sources = getFiles();

		if (params == null)
			params = [];

		if (opts.outputDir == null)
			opts.outputDir = "";
		else if (!StringTools.endsWith(opts.outputDir, "/"))
			opts.outputDir += "/";

		var hasOpt = false;
		for (p in params)
			if (p.substr(0, 2) == "-O")
				hasOpt = true;
		if (!hasOpt)
			params.push("-O2");

		var lib = opts.nativeLib;

		var emSdk = Sys.getEnv("EMSCRIPTEN");
		if (emSdk == null)
			throw "Missing EMSCRIPTEN environment variable. Install emscripten";
		var emcc = emSdk + "/emcc";

		// build sources BC files
		var outFiles = [];

		for (cfile in sources) {
			var out = opts.outputDir + cfile.substr(0, -4) + ".bc";
			var args = params.concat(["-c", cfile, "-o", out]);
			command(emcc, args);
			outFiles.push(out);
		}

		// link : because too many files, generate Makefile
		var tmp = opts.outputDir + "Makefile.tmp";
		var args = params.concat([
			"-s",
			'EXPORT_NAME="\'$lib\'"',
			"-s",
			"MODULARIZE=1",
			"--memory-init-file",
			"0",
			"-lembind",
			"-o",
			'$lib.js'
		]);
		var output = "SOURCES = " + outFiles.join(" ") + "\n";
		output += "all:\n";
		output += "\t" + emcc + " $(SOURCES) " + args.join(" ");
		sys.io.File.saveContent(tmp, output);
		command("make", ["-f", tmp]);
		sys.FileSystem.deleteFile(tmp);



	}

	static function command(cmd, args:Array<String>) {
		Sys.println("> " + cmd + " " + args.join(" "));
		var ret = Sys.command(cmd, args);
		if (ret != 0)
			throw "Command '" + cmd + "' has exit with error code " + ret;
	}

}
#end