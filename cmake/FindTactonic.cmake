# This module looks for Tactonic's pressure sensor libraries.
# If cmake isn't finding these for you, check that the Tactonic_DIR
# environment variable is defined and it contains include/Tactonic.h
# and lib/Tactonic.lib.

find_path(Tactonic_INCLUDE_DIR
    Tactonic.h
    HINTS "$ENV{HOME}/tactonic/include"
    )

find_library(Tactonic_LIBRARY
    tactonic
    HINTS "$ENV{HOME}/tactonic/lib"
    )

find_library(TactonicTouch_LIBRARY
    tactonicTouch
    HINTS "$ENV{HOME}/tactonic/lib"
    )

set(Tactonic_LIBRARIES "${Tactonic_LIBRARY}" "${TactonicTouch_LIBRARY}")
set(Tactonic_INCLUDE_DIRS "${Tactonic_INCLUDE_DIR}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Tactonic DEFAULT_MSG Tactonic_LIBRARY
    TactonicTouch_LIBRARY Tactonic_INCLUDE_DIR)

mark_as_advanced(Tactonic_INCLUDE_DIR Tactonic_LIBRARY)

