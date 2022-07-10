# Heaps Ozz



[Ozz Animation](http://guillaumeblanc.github.io/ozz-animation/) wrapper for Heaps.

Designed to support both HL and JS via [WebIDL](https://github.com/ncannasse/webidl) (Currently only HL)

## Compilation
First, run the IDL genenerator to build ozz.cpp. Then build via cmake. Currently requires a custom fork of WebIDL.

### Hashlink
```
haxe -lib webidl --macro "ozz.Generator.generateCpp()"
set HASHLINKPATH=C:\HaxeToolkit\hl
cmake .
cmake --build .
```

### Javascript

TODO