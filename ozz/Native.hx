package ozz;

typedef Native = haxe.macro.MacroType<[webidl.Module.build({ idlFile : "ozz/ozz.idl", chopPrefix : "ozz", autoGC : true, nativeLib : "ozz" })]>;
