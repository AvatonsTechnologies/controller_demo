find_path(vjoy_INCLUDE_DIR
    public.h
#    HINTS "$ENV{HOME}/vjoy/inc"
    HINTS "vjoy/inc"
    )

find_library(vjoy_LIBRARY
    vJoyInterface
#    HINTS "$ENV{HOME}/vjoy/lib"
    HINTS "vjoy/lib"
    )

set(vjoy_LIBRARIES "${vjoy_LIBRARY}")
set(vjoy_INCLUDE_DIRS "${vjoy_INCLUDE_DIR}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(vjoy DEFAULT_MSG vjoy_LIBRARY
    vjoy_INCLUDE_DIR)

mark_as_advanced(vjoy_INCLUDE_DIR vjoy_LIBRARY)

