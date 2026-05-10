"""
DeadLock - A Package Manager for Data Scientists
"""
__version__ = "1.0.0"

from . import deadlock_core

DL_Extension = deadlock_core.DL_Wrapper
__all__ = ["DL_Extension"]
