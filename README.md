# Heaps Ozz

[Ozz Animation](http://guillaumeblanc.github.io/ozz-animation/) integration for Heaps. This adds a more fully featured animation system, as well as offline model converters to a new format (ie, no longer uses the HMD format)

This library's current status is "Extremely cursed", and is not recommended for use in any project. Extremely open to PRs though!

Supports both HL and JS targets via the magic of [Emscripten](https://emscripten.org/)

## Compilation
No particularly exciting dependencies. If you want to import FBX models, you'll need to drop in a copy of the FBX SDK. Otherwise you can still use gltf.

### Hashlink
```
set HASHLINKPATH=C:\HaxeToolkit\hl
cmake .
cmake --build .
```

### Javascript
You're going to need emscripten. emsdk makes this surprisingly painless to setup.
You also need cmake, and regular make too, because I hate you.

The HL build must be run first, as it generates the vcproj files that emscripten uses to get a file list. It won't actually compile right now but that's ok, we only really needed the cmake output. If cmake isn't using VS (and thus not generating vcproj files) it won't compile. Someone should probably fix this.

```
haxe --macro "ozz.Generator.generateJs()"
```
