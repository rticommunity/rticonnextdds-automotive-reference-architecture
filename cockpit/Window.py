
# WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

# This file was generated from Window.idl
# using RTI Code Generator (rtiddsgen) version 4.3.0.
# The rtiddsgen tool is part of the RTI Connext DDS distribution.
# For more information, type 'rtiddsgen -help' at a command shell
# or consult the Code Generator User's Manual.

from dataclasses import field
from typing import Union, Sequence, Optional
import rti.idl as idl
from enum import IntEnum
import sys
import os


TOPIC_WINDOW_COMMAND = "WindowCommand"

TOPIC_WINDOW_UPDATE = "WindowUpdate"

ID_STR_LEN = 2

window_id_t = str

@idl.struct(
    member_annotations = {
        'id': [idl.key, idl.bound(ID_STR_LEN)],
    }
)
class WindowCommand:
    id: str = ""
    position: idl.uint16 = 0

@idl.struct(
    member_annotations = {
        'id': [idl.key, idl.bound(ID_STR_LEN)],
    }
)
class WindowUpdate:
    id: str = ""
    position: idl.uint16 = 0
