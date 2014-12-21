libtransmogrify
===============

Playground for gtest, cmake serves as demo for a buildroot package

## hPossible BUILD_TYPE(s):
  Debug  
  Release (default
  RelWithDebInfo

## Options & Defaults (compilation flags & targets):
-DBUILD_TEST=OFF      (build google test)
-DBUILD_GTEST_FROM_SRC=Off (recompile gtest as external project)
-DMAGIC_MOJO=19       (some compile time int)
-DPROFILE=Off         (profiling)
-DTEST_COVERAGE=Off   (code coverage)
-DINSTALL_UNIT_TEST_ON_TARGET=Off (install unittest on target rootfs)

 
