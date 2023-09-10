set(PROJECT_NAME PlayStation4Rendering)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "PS4MemoryAware.h"
    "PS4Frame.h"
    "PS4Mesh.h"
    "PS4RendererBase.h"
    "PS4Shader.h"
    "PS4Texture.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "PS4MemoryAware.cpp"
    "PS4Frame.cpp"
    "PS4Mesh.cpp"
    "PS4RendererBase.cpp"
    "PS4Shader.cpp"
    "PS4Texture.cpp"
)
source_group("Source Files" FILES ${Source_Files})

#set(GLAD
#    "glad/gl.h"
#    "glad/gl.c"
#)
#source_group("GLAD" FILES ${GLAD})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
#    ${GLAD}
)

set_source_files_properties(${ALL_FILES} PROPERTIES LANGUAGE CXX)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE PlayStation4Rendering)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)
################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    ";"
    "UNICODE;"
    "_UNICODE"
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:
            /Oi;
            /Gy
        >
        /permissive-;
        /std:c++latest;
        /sdl;
        /W3;
        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        ${DEFAULT_CXX_EXCEPTION_HANDLING};
        /Y-
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF
        >
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:
            -Oi;
            -Gy
        >
        $<$<CONFIG:Debug>:
            -O0;
        >
        -frtti;
        -fexceptions;
        -std=c++17;
        #${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        #${DEFAULT_CXX_EXCEPTION_HANDLING};
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:
            -OPT:REF;
            -OPT:ICF
        >
    )
#else()
#    target_compile_options(${PROJECT_NAME} PRIVATE
#        $<$<CONFIG:Release>:
#            -Oi;
#            -Gy
#        >
#        -permissive-;
#        -std:c++17;
#        -sdl;
#        -W3;
#        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
#        ${DEFAULT_CXX_EXCEPTION_HANDLING};
#        -Y-
#    )
#    target_link_options(${PROJECT_NAME} PRIVATE
#        $<$<CONFIG:Release>:
#            -OPT:REF;
#            -OPT:ICF
#        >
#    )
endif()

################################################################################
# Dependencies
################################################################################
set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "NCLCoreClasses"
)
include_directories("../NCLCoreClasses/")
include_directories("../Playstation4Rendering/")

target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")
