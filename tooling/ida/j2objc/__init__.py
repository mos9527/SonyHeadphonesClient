"""Sound Connect J2ObjC MDR protocol extractor.

The model and render modules intentionally remain importable outside IDA.
"""

from .model import SCHEMA_VERSION

__all__ = ["SCHEMA_VERSION"]
