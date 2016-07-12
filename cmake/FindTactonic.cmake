# This module looks for Tactonic's pressure sensor libraries.

find_path(Tactonic_INCLUDE_DIR
    Tactonic.h
    HINTS "tactonic/include"
          "$ENV{HOME}/tactonic/include"
    )

find_library(Tactonic_LIBRARY
    tactonic
    HINTS "tactonic/lib"
          "$ENV{HOME}/tactonic/lib"
    )

find_library(TactonicTouch_LIBRARY
    tactonicTouch
    HINTS "tactonic/lib"
          "$ENV{HOME}/tactonic/lib"
    )

set(Tactonic_LIBRARIES "${Tactonic_LIBRARY}" "${TactonicTouch_LIBRARY}")
set(Tactonic_INCLUDE_DIRS "${Tactonic_INCLUDE_DIR}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Tactonic DEFAULT_MSG Tactonic_LIBRARY
    TactonicTouch_LIBRARY Tactonic_INCLUDE_DIR)

mark_as_advanced(Tactonic_INCLUDE_DIR Tactonic_LIBRARY)

