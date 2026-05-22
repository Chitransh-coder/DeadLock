# Test runner for DeadLock

This script lets you exercise the already-built Python extension module for DeadLock without rebuilding the C++ project.

How it works

- It loads the compiled extension from `build/Release/deadlock_core*.pyd` using Python's importlib and calls several lightweight methods (timestamp, python availability, PyPI queries).

Run

Open a terminal in the repository root and run:

```bash
python -m tests.run_tests
```

Notes

- Build the project once so `build/Release/deadlock_core.cp312-win_amd64.pyd` exists. Subsequent runs of this script do not require rebuilding.
- Some tests perform network requests and may fail if offline.
