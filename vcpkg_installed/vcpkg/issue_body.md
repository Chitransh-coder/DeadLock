Package: libb2:x64-linux@0.98.1#7

**Host Environment**

- Host: x64-linux
- Compiler: GNU 16.1.1
- CMake Version: 4.3.3
-    vcpkg-tool version: 2026-05-27-d5b6777d666efc1a7f491babfcdab37794c1ae3e
    vcpkg-scripts version: f87344cac0 2026-07-11 (32 hours ago)

**To Reproduce**

`vcpkg install `

**Failure logs**

```
Downloading https://github.com/BLAKE2/libb2/archive/2c5142f12a2cd52f3ee0a43e50a3a76f75badf85.tar.gz -> BLAKE2-libb2-2c5142f12a2cd52f3ee0a43e50a3a76f75badf85.tar.gz
Successfully downloaded BLAKE2-libb2-2c5142f12a2cd52f3ee0a43e50a3a76f75badf85.tar.gz
-- Extracting source /home/chitranshsoral/Projects/DeadLock/vcpkg/downloads/BLAKE2-libb2-2c5142f12a2cd52f3ee0a43e50a3a76f75badf85.tar.gz
-- Using source at /home/chitranshsoral/Projects/DeadLock/vcpkg/buildtrees/libb2/src/6f75badf85-b3a5d32c8d.clean
-- Getting CMake variables for x64-linux
-- Loading CMake variables from /home/chitranshsoral/Projects/DeadLock/vcpkg/buildtrees/libb2/cmake-get-vars_C_CXX-x64-linux.cmake.log
CMake Error at /home/chitranshsoral/Projects/DeadLock/vcpkg_installed/x64-linux/share/vcpkg-make/vcpkg_make.cmake:108 (message):
  libb2 currently requires the following programs from the system package
  manager:

      autoconf autoconf-archive automake libtoolize



      On Debian and Ubuntu derivatives:
          sudo apt install autoconf autoconf-archive automake libtool
      On recent Red Hat and Fedora derivatives:
          sudo dnf install autoconf autoconf-archive automake libtool
      On Arch Linux and derivatives:
          sudo pacman -S autoconf autoconf-archive automake libtool
      On Alpine:
          apk add autoconf autoconf-archive automake libtool
      On macOS:
          brew install autoconf autoconf-archive automake libtool

Call Stack (most recent call first):
  /home/chitranshsoral/Projects/DeadLock/vcpkg_installed/x64-linux/share/vcpkg-make/vcpkg_make_configure.cmake:66 (vcpkg_run_autoreconf)
  buildtrees/versioning_/versions/libb2/4bf555d778e96bde29715be0519297dd805b9d2b/portfile.cmake:9 (vcpkg_make_configure)
  scripts/ports.cmake:206 (include)



```

**Additional context**

<details><summary>vcpkg.json</summary>

```
{
  "$schema": "https://raw.githubusercontent.com/microsoft/vcpkg-tool/main/docs/vcpkg.schema.json",
  "dependencies": [
    {
      "name": "curl",
      "features": [
        "ssl"
      ]
    },
    "zlib",
    "pybind11"
  ],
  "builtin-baseline": "75cabb002b38ee946bb9e05245c4f984d93a6f3c"
}

```
</details>
