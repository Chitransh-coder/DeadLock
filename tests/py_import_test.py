import sys
import os
import fnmatch
import importlib

root = 'build'
found = None
for dirpath, _, files in os.walk(root):
    for f in files:
        if fnmatch.fnmatch(f, 'deadlock_core*.pyd') or fnmatch.fnmatch(f, 'deadlock_core*.so') or fnmatch.fnmatch(f, 'deadlock_core*.dll'):
            found = os.path.join(dirpath, f)
            break
    if found:
        break

print('found:', found)
if not found:
    print('Built module not found under', root)
    sys.exit(2)

sys.path.insert(0, os.path.dirname(found))
try:
    m = importlib.import_module('deadlock_core')
    print('Imported module:', m)
    attrs = [a for a in dir(m) if not a.startswith('_')]
    print('module attrs:', attrs)
    print('Package present:', 'Package' in attrs)
    print('DL_Wrapper present:', 'DL_Wrapper' in attrs)
    print('Package object:', getattr(m, 'Package', None))
except Exception as e:
    print('Import failed:', e)
    raise
