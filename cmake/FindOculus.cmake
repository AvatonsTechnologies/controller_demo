find_path(Oculus_INCLUDE_DIR
    OVR.h
    HINTS "$ENV{HOME}/OculusSDK/LibOVR/Include"
    )

find_library(Oculus_LIBRARY
    LibOVR
    HINTS "$ENV{HOME}/OculusSDK/LibOVR/Lib/Windows/Win32/Release/VS2015"
    )

set(Oculus_LIBRARIES "${Oculus_LIBRARY}")
set(Oculus_INCLUDE_DIRS "${Oculus_INCLUDE_DIR}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Oculus DEFAULT_MSG Oculus_LIBRARY
    Oculus_INCLUDE_DIR)

mark_as_advanced(Oculus_INCLUDE_DIR Oculus_LIBRARY)
