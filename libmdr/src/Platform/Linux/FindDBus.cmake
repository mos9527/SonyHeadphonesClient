# - Try to find DBus
# Once done, this will define
#
#  DBus_FOUND - system has DBus
#  DBus_INCLUDE_DIRS - the DBus include directories
#  DBus_LIBRARIES - link these to use DBus
#
# Copyright (C) 2012 Raphael Kubo da Costa <rakuco@webkit.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1.  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
# 2.  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND ITS CONTRIBUTORS ``AS
# IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR ITS
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(PC_DBus QUIET dbus-1)

FIND_LIBRARY(DBus_LIBRARIES
    NAMES dbus-1
    HINTS ${PC_DBus_LIBDIR}
          ${PC_DBus_LIBRARY_DIRS}
)

FIND_PATH(DBus_INCLUDE_DIR
    NAMES dbus/dbus.h
    HINTS ${PC_DBus_INCLUDEDIR}
          ${PC_DBus_INCLUDE_DIRS}
)

GET_FILENAME_COMPONENT(_DBus_LIBRARY_DIR ${DBus_LIBRARIES} PATH)
FIND_PATH(DBus_ARCH_INCLUDE_DIR
    NAMES dbus/dbus-arch-deps.h
    HINTS ${PC_DBus_INCLUDEDIR}
          ${PC_DBus_INCLUDE_DIRS}
          ${_DBus_LIBRARY_DIR}
          ${DBus_INCLUDE_DIR}
    PATH_SUFFIXES include
)

SET(DBus_INCLUDE_DIRS ${DBus_INCLUDE_DIR} ${DBus_ARCH_INCLUDE_DIR})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DBus REQUIRED_VARS DBus_INCLUDE_DIRS DBus_LIBRARIES)
