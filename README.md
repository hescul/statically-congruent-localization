# Statically Congruent Localization
*Author*: Minh Nguyen, Lam Luu

## Build Instruction
Requires:
- CMake: `3.25` or higher.
- Ninja: `1.11.1` or higher.
- g++ and gcc (at least 12.2.0) is included in the system's PATH environment variable.
- [vcpkg](https://vcpkg.io/en/index.html) with OpenCV installed.

When installing OpenCV with vcpkg, make sure the triplet is set correctly. For example:
```commandline
vcpkg install opencv:x64-windows
```

Inside the directory `statically-congruent-localization`, execute the following commands:

```commandline
cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build build --target scl -j 10
```
When done, run the executable `scl` produced by the build to run the program.