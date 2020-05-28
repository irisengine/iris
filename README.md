# IRIS
Iris is a cross-platform game engine written in modern C++

## Design
Iris is split into components, each of which provides a cross-platform
abstraction over native and third-party libraries.

| Component | Description |
|-|-|
|[Core](./src/core)|Core classes for the library (e.g. maths)|
|[Graphics](./src/graphics)|Rendering entities|
|[Jobs](./src/jobs)|Task parallelism|
|[Log](./src/log)|Logging framework|
|[Platform](./src/platform)|Platform specific classes (e.g. windowing)|

## Building
Iris uses cmake as a build system generator and builds as a static library. By
default Iris will build for the current platform using sane defaults.

```bash
mkdir build
cd build
cmake ..
make -j8
```

### Build options
The following table details the flags that can be passed to cmake during build

| Flag | Values | Description |
|-|-|-|
|`-DIRIS_PLATFORM` | `MACOS`, `IOS` | Which platform to build for |
|`-DIRIS_GRAPHICS_API` | `METAL`, `OPENGL` | Which graphics API to use |
|`-DIRIS_JOBS_API` | `FIBERS`, `THREADS` | Which jobs API to use |
|`-DIRIS_BUNDLE_IDENTIFIER` | | Preface string for sample bundle identifiers |
|`-DIRIS_DEVLOPMENT_TEAM` | | iOS development team id |

Not all flags are valid for all platforms, to make things easier also supplied
are scripts for each platform which provide finer grained control and details
over the various options.

* [`./build_ios.sh`](./build_ios.sh)
* [`./build_macos.sh`](./build_macos.sh)

## [Samples](./samples)
The samples provide various example usages of the library.

## Tests
After building the unit tests can be run as:
```bash
./unit_tests
```

Note that the unit tests only tests the implementations and APIs set at build
time e.g. if fibers was not configured as the jobs API then none of the fiber
code will be tested.

