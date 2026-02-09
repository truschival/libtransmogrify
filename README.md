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
conan install  . -pr gcc -pr coverage --build=missing -c tools.cmake.cmaketoolchain:generator="Unix Makefiles" -o shared=False -o BUILD_TEST=True

cmake -B build/Debug -S . -DBUILD_TEST=On
cmake --build  build/Debug -t 
cmake --build  build/Debug -t coverage_report
```

**Note:** use static libraries for coverage build  

### Debian Packages for llvm

To use code coverage with llvm-cov you need to install

* `libclang-rt-dev`
* `llvm`
* `llvm-runtime`