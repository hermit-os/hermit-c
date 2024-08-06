# Hermit for C

This repository provides information about using [the Hermit kernel] with C along with some test applications.

[the Hermit kernel]: https://github.com/hermit-os/kernel

For building C code for Hermit you need a corresponding cross compiler.
We provide a Docker image containing a GCC-based cross compiler in our [hermit-gcc] repository.
With this cross compiler you can compile most C code for Hermit.

When compiling code with the Hermit cross compiler, you need to enable position-independent code (PIC) or position-independent executable (PIE).
When linking an application, you need to explicitly link against a Hermit kernel (`libhermit.a`) that you can build from our [kernel] repository.
Make sure to link against the *whole archive* ([`--whole-archive`]).

[hermit-gcc]: https://github.com/hermit-os/hermit-gcc
[kernel]: https://github.com/hermit-os/kernel

## GCC

You can compile your C applications manually with Hermit's cross compiler by linking against the Hermit kernel.
To generate PIE code, add [`-fPIE`].
To also link to a PIE, add [`-pie`].
Link against the whole kernel archive using [`--whole-archive`] to avoid an error about undefined kernel symbols.
A complete GCC invocation might look like this:

```bash
x86_64-hermit-gcc -o main -fPIE main.c -pie -Wl,--whole-archive build-x86_64-hermit-debug/libhermit.a -Wl,--no-whole-archive
```

[`-fPIE`]: https://gcc.gnu.org/onlinedocs/gcc/Code-Gen-Options.html
[`-pie`]: https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html
[`--whole-archive`]: https://sourceware.org/binutils/docs/ld/Options.html

## Meson

This repository uses the [Meson build system] to maintain a set of C-based test applications.
If your project uses Meson, you should be able to use the [cross files] from this repository.
You can also use this project's `hermit` target definition ([`meson.build`]) for automatically building a Hermit kernel to link against.

[Meson build system]: https://mesonbuild.com
[cross files]: cross
[`meson.build`]: meson.build

To build the test applications from this repository, first set up a build directory:

```bash
meson setup --cross-file cross/x86_64-hermit.ini build-x86_64-hermit-debug
```

Then, compile the project:

```bash
meson compile -C build-x86_64-hermit-debug -v
```

You can then run the resulting Hermit applications using [the Hermit loader] or in [Uhyve], for example:

```bash
qemu-system-x86_64 \
    -enable-kvm \
    -cpu host \
    -smp 1 \
    -m 1024M \
    -device isa-debug-exit,iobase=0xf4,iosize=0x04 \
    -display none -serial stdio \
    -kernel hermit-loader-x86_64 \
    -initrd build-x86_64-hermit-debug/hello_world
```

[the Hermit loader]: https://github.com/hermit-os/loader
[Uhyve]: https://github.com/hermit-os/uhyve

## CMake

When using CMake, you can use this [toolchain file]:

[toolchain file]: https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html

```cmake
# x86_64-hermit.cmake

set(CMAKE_SYSTEM_NAME Hermit)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER x86_64-hermit-gcc)
set(CMAKE_CXX_COMPILER x86_64-hermit-g++)

# Needed to pass CMake's compiler test during build system generation
set(CMAKE_EXE_LINKER_FLAGS_INIT /absolute/path/to/libhermit.a)
```

The toolchain file can be supplied to CMake during configuration:

```bash
cmake -G Ninja -B build-x86_64-hermit-debug -S . --toolchain x86_64-hermit.cmake
```

You can then build as usual with CMake:

```bash
cmake --build build-x86_64-hermit-debug
```

## Rust applications with C code

When building a Rust application with C code that uses the C standard library, enable the `hermit` crate's `libc` feature to link against the C standard library.

### cc-rs

For Rust applications with [cc-rs], set the following environment variables to use Hermit's cross compiler:

```bash
export CC_x86_64_unknown_hermit=x86_64-hermit-gcc
export CXX_x86_64_unknown_hermit=x86_64-hermit-g++
export AR_x86_64_unknown_hermit=x86_64-hermit-gcc-ar
```

[cc-rs]: https://docs.rs/cc

### cmake-rs

For Rust applications with [cmake-rs], provide the toolchain file via this environment variable:

```bash
export CMAKE_TOOLCHAIN_FILE_x86_64_unknown_hermit="$PWD/hermit-toolchain.cmake"
```

[cmake-rs]: https://docs.rs/cmake

### rust-bindgen

When building a Rust application that uses [`bindgen`], you currently need to manually specify the Hermit's system search path.
First, find Hermit's search directory list for Hermit's C preprocessor (cpp):

```bash
x86_64-hermit-cpp -v /dev/null -o /dev/null
```

Then, provide these directories to `bindgen`'s Clang by adding `-isystem` arguments to this environment variable:

```bash
export BINDGEN_EXTRA_CLANG_ARGS_x86_64_unknown_hermit="-isystem /opt/hermit/lib/gcc/x86_64-hermit/7.5.0/include -isystem /opt/hermit/lib/gcc/x86_64-hermit/7.5.0/include-fixed -isystem /opt/hermit/lib/gcc/x86_64-hermit/7.5.0/../../../../x86_64-hermit/include"
```

[`bindgen`]: https://rust-lang.github.io/rust-bindgen

## License

Licensed under either of

 * Apache License, Version 2.0
   ([LICENSE-APACHE](LICENSE-APACHE) or http://www.apache.org/licenses/LICENSE-2.0)
 * MIT license
   ([LICENSE-MIT](LICENSE-MIT) or http://opensource.org/licenses/MIT)

at your option.

### Contribution

Unless you explicitly state otherwise, any contribution intentionally submitted
for inclusion in the work by you, as defined in the Apache-2.0 license, shall be
dual licensed as above, without any additional terms or conditions.
