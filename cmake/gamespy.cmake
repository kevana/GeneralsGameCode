set(GS_OPENSSL FALSE)
set(GAMESPY_SERVER_NAME "server.cnc-online.net")

FetchContent_Declare(
    gamespy
    GIT_REPOSITORY https://github.com/TheAssemblyArmada/GamespySDK.git
    GIT_TAG        07e3d15c500415abc281efb74322ab6d9c857eb8
)

FetchContent_MakeAvailable(gamespy)

# Add platform defines required by GameSpy source code on macOS/Linux
if(APPLE)
    set(_gs_platform_def _MACOSX)
elseif(UNIX)
    set(_gs_platform_def _LINUX)
endif()
if(_gs_platform_def)
    if(TARGET gsinterface)
        target_compile_definitions(gsinterface INTERFACE ${_gs_platform_def})
    endif()
    foreach(_gs_tgt gscommon gamespy)
        if(TARGET ${_gs_tgt})
            target_compile_definitions(${_gs_tgt} PRIVATE ${_gs_platform_def})
        endif()
    endforeach()
endif()
