#!/usr/bin/env python3
import fnmatch
import importlib.util
import os
import sys


def load_extension_from_path(path):
    if not os.path.exists(path):
        print("Extension not found:", path)
        return None
    spec = importlib.util.spec_from_file_location("deadlock_core", path)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def try_attr(obj, *names, default=None):
    for n in names:
        if hasattr(obj, n):
            return getattr(obj, n)
    return default


def main():
    repo_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    ext_path = None
    for dirpath, _, files in os.walk(os.path.join(repo_root, "build")):
        for filename in files:
            if fnmatch.fnmatch(filename, "deadlock_core*.pyd") or fnmatch.fnmatch(filename, "deadlock_core*.so") or fnmatch.fnmatch(filename, "deadlock_core*.dll"):
                ext_path = os.path.join(dirpath, filename)
                break
        if ext_path:
            break

    if not ext_path:
        print("No built extension (.pyd) found in build/Release. Build the project once and re-run this script.")
        sys.exit(2)

    mod = load_extension_from_path(ext_path)
    if mod is None:
        print("Failed to load extension module")
        sys.exit(1)

    print("Loaded module:", mod)

    # Try to find wrapper class
    Wrapper = try_attr(mod, "DL_Wrapper", "DL_Extension", default=None)
    if Wrapper is None:
        print("No DL_Wrapper/DL_Extension class exported by module. Listing attributes:")
        print(sorted(dir(mod)))
        sys.exit(0)

    dl = Wrapper()

    # Helper to call member with multiple name options
    def call(dl_obj, *attr_names_and_args):
        # last items without '=' are args; attr names are strings until a non-string appears
        attr_names = []
        args = []
        for it in attr_names_and_args:
            if isinstance(it, str):
                attr_names.append(it)
            else:
                args.append(it)
        for n in attr_names:
            if hasattr(dl_obj, n):
                return getattr(dl_obj, n)(*args)
        raise AttributeError(f"None of {attr_names} found on object")

    # Run a few lightweight checks that won't modify system state
    try:
        is_py = call(dl, "is_python_available", "isPythonAvailable")
        print("isPythonAvailable:", is_py)
    except Exception as e:
        print("isPythonAvailable: error ->", e)

    try:
        ts = call(dl, "get_current_timestamp", "getCurrentTimestamp")
        print("getCurrentTimestamp:", ts)
    except Exception as e:
        print("getCurrentTimestamp: error ->", e)

    try:
        path = call(dl, "get_deadlock_file_path", "getDeadLockFilePath")
        print("dead.lock path:", path)
    except Exception as e:
        print("getDeadLockFilePath: error ->", e)

    # Try safe network queries (may fail if offline)
    try:
        latest = call(dl, "get_latest_version", "getLatestVersion", "requests")
        print("requests latest version:", latest)
    except Exception as e:
        print("getLatestVersion: error ->", e)

    try:
        deps = call(dl, "get_package_dependencies", "getPackageDependencies", "requests")
        print("requests dependencies:", deps)
    except Exception as e:
        print("getPackageDependencies: error ->", e)


if __name__ == "__main__":
    main()
