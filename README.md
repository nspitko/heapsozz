# Heaps Ozz

[Ozz Animation](http://guillaumeblanc.github.io/ozz-animation/) integration for Heaps. Requires use of its own model tools, which I haven't gotten around to compiling yet, so just use the sample models for now.

This library's current status is "Extremely cursed". Code here should not be viewed by persons under the age of 18 (or anyone really).

Designed to support both HL and JS via [WebIDL](https://github.com/ncannasse/webidl) (Currently only ~~HL~~ JS)

## Compilation
Build via cmake. Currently requires a custom fork of WebIDL, but no longer actually uses WebIDL, so don't worry about it.

### Hashlink
```
set HASHLINKPATH=C:\HaxeToolkit\hl
cmake .
cmake --build .
```

### Javascript
You're going to need emscripten. emsdk makes this surprisingly painless to setup.
You also need cmake, and regular make too, because I hate you.

The HL build must be run first, as it generates the vcproj files that emscripten uses to get a file list. It won't actually compile right now but that's ok, we only really needed the cmake output. If cmake isn't using VS (and thus not generating vcproj files) it won't compile.

```
haxe --macro "ozz.Generator.generateJs()"
```