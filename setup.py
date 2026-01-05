import os
import subprocess
import sys
from pathlib import Path
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=""):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)

class CMakeBuild(build_ext):
    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))

        # Bootstrap vcpkg if needed
        vcpkg_root = os.path.join(ext.sourcedir, "vcpkg")
        vcpkg_exe = os.path.join(vcpkg_root, "vcpkg.exe" if sys.platform == "win32" else "vcpkg")

        if not os.path.exists(vcpkg_exe):
            print("Bootstrapping vcpkg...")
            if not os.path.exists(vcpkg_root):
                subprocess.check_call(
                    ["git", "clone", "https://github.com/Microsoft/vcpkg.git", vcpkg_root]
                )
            bootstrap_script = "bootstrap-vcpkg.bat" if sys.platform == "win32" else "bootstrap-vcpkg.sh"
            subprocess.check_call(
                [os.path.join(vcpkg_root, bootstrap_script)],
                cwd=vcpkg_root
            )

        vcpkg_toolchain = os.path.join(vcpkg_root, "scripts", "buildsystems", "vcpkg.cmake")

        cmake_args = [
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}",
            f"-DPYTHON_EXECUTABLE={sys.executable}",
            f"-DCMAKE_TOOLCHAIN_FILE={vcpkg_toolchain}",
            "-DVCPKG_TARGET_TRIPLET=x64-windows-static" if sys.platform == "win32" else "-DVCPKG_TARGET_TRIPLET=x64-linux",
            "-DCMAKE_BUILD_TYPE=Release",
        ]

        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)

        # Configure
        subprocess.check_call(
            ["cmake", ext.sourcedir] + cmake_args,
            cwd=self.build_temp
        )

        # Build
        subprocess.check_call(
            ["cmake", "--build", "."],
            cwd=self.build_temp
        )

setup(
    ext_modules=[CMakeExtension("_deadlock")],
    cmdclass={"build_ext": CMakeBuild},
)