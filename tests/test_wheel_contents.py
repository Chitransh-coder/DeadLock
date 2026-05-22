"""
Test to verify that all required DLLs and the .pyd module are present in the built wheel.
"""
import zipfile
import sys
from pathlib import Path


def test_wheel_contents():
    """Verify that the wheel contains deadlock_core.pyd, libcurl.dll, and zlib1.dll."""


    dist_dir = Path(__file__).parent.parent / "dist"
    wheel_files = list(dist_dir.glob("*.whl"))

    if not wheel_files:
        print("No wheel file found")
        return False

    wheel_path = wheel_files[0]
    print(f"Testing wheel: {wheel_path.name}")

    required_dlls = [
        "deadlock/libcurl.dll",
        "deadlock/zlib1.dll",
    ]

    try:
        with zipfile.ZipFile(wheel_path, 'r') as whl:
            wheel_contents = whl.namelist()

            print(f"\nWheel contents ({len(wheel_contents)} items):")
            for item in sorted(wheel_contents):
                print(f"  - {item}")

            print("\nVerifying required artifacts:")
            all_present = True

            pyd_files = [item for item in wheel_contents if item.startswith("deadlock/deadlock_core") and item.endswith(".pyd")]
            if pyd_files:
                print(f"  True : {pyd_files[0]}")
            else:
                print("  False : deadlock/deadlock_core*.pyd")
                all_present = False

            for artifact in required_dlls:
                found = artifact in wheel_contents
                print(f"  {found} : {artifact}")
                if not found:
                    all_present = False

            if all_present:
                print("\nSUCCESS: All required artifacts are present")
                return True
            else:
                print("\nFAILED: Required artifacts are missing")
                return False

    except zipfile.BadZipFile:
        print(f"FAILED: {wheel_path} is not a valid file")
        return False
    except Exception as e:
        print(f"FAILED: Error while reading wheel: {e}")
        return False


if __name__ == "__main__":
    success = test_wheel_contents()
    sys.exit(0 if success else 1)
