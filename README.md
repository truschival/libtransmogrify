# libtransmogrify

Playground for gtest, cmake serves as demo for a buildroot package

## Possible BUILD_TYPE(s)

- Debug
- Release (default)
- RelWithDebInfo

## Options & Defaults

- -DBUILD_TEST=OFF   (build with unit tests)
- -DMAGIC_MOJO=19    (some compile time int)

## Build

```shell
conan install  . -pr gcc -pr coverage --build=missing -c tools.cmake.cmaketoolchain:generator="Unix Makefiles"  -o BUILD_TEST=True


cmake -B build/Debug -S . -DBUILD_TEST=On
cmake --build  build/Debug -t 
cmake --build  build/Debug -t coverage_report
``` 





