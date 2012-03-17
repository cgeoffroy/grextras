# Copyright 2011-212 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Include swig generation macros
########################################################################
find_package(SWIG)
find_package(PythonLibs)
if(NOT SWIG_FOUND OR NOT PYTHONLIBS_FOUND)
    return()
endif()
include(GrSwig)
include(GrPython)

########################################################################
# Setup swig generation
########################################################################
set(GR_SWIG_INCLUDE_DIRS
    ${GR_EXTRAS_INCLUDE_DIRS}
)

foreach(incdir ${GNURADIO_CORE_INCLUDE_DIRS})
    list(APPEND GR_SWIG_INCLUDE_DIRS ${incdir}/swig)
endforeach(incdir)

set(GR_SWIG_LIBRARIES gnuradio-extras)

GR_SWIG_MAKE(extras_swig extras_swig.i)

########################################################################
# Install the built swig module
########################################################################
GR_SWIG_INSTALL(
    TARGETS extras_swig
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/extras
    COMPONENT "extras_python"
)

########################################################################
# Install swig .i files for development
########################################################################
install(
    FILES
    extras_swig.i
    extras_delay.i
    extras_sources.i
    extras_stream_selector.i
    extras_ops.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "extras_swig"
)