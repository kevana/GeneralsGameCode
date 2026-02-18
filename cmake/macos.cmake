# macOS specific compiler and linker configurations

if(APPLE)
    message(STATUS "Configuring macOS build settings")

    # Set minimum deployment target
    set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum macOS deployment target")
    message(STATUS "macOS deployment target: ${CMAKE_OSX_DEPLOYMENT_TARGET}")

    # Compatibility flags for legacy code
    add_compile_options(
        -fno-strict-aliasing        # Avoid type-punning issues with legacy casts
    )

    # MSVC compatibility macros for macOS/Clang
    add_compile_definitions(
        __forceinline=inline\ __attribute__\(\(always_inline\)\)
        _USE_MATH_DEFINES
    )

    # Frameworks commonly needed
    # (Individual targets will link specific frameworks as needed)

    message(STATUS "macOS configuration complete")
endif()
