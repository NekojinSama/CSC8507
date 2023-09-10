set(PROJECT_NAME CSC8503)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "SoundDevice.h"
    "Sounds.h"
    "SoundsManager.h"
    "SoundSource.h"
    "SoundSystem.h"
    "StateGameObject.h"
    "MainMenu.h"
    "TutorialGame.h"
    "Player.h"
    "Objective.h"
    "LevelBuilder.h"
    "AssetManager.h"
    "CoopGame.h"
    "HUD.h"
    "Controller.h"
    "Gamepad.h"
    "KeyboardController.h"
    "RendererFactory.h"
    "ConfigOptions.h"
    "AIEntity.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "Main.cpp"
    "dr_wav.cpp"
    "SoundDevice.cpp"
    "Sounds.cpp"
    "SoundsManager.cpp"
    "SoundSource.cpp"
    "SoundSystem.cpp"
    "StateGameObject.cpp"
    "MainMenu.cpp"
    "TutorialGame.cpp"
    "Player.cpp"
    "Objective.cpp"
    "LevelBuilder.cpp"
    "AssetManager.cpp"
    "CoopGame.cpp"
    "Controller.cpp"
    "HUD.cpp"
    "Gamepad.cpp"
    "KeyboardController.cpp"
    "RendererFactory.cpp"
    "ConfigOptions.cpp"
    "AIEntity.cpp"
)

if(USE_VULKAN)
list(APPEND Header_Files "GameTechVulkanRenderer.h")
list(APPEND Source_Files "GameTechVulkanRenderer.cpp")
# list(APPEND Source_Files "GameTechVulkanRenderer.cpp")

file(GLOB SHADER_FILES ${ASSET_ROOT}/Shaders/VK/*.*)
endif()

if(WIN32)
	list(APPEND Header_Files "GameTechRenderer.h")
	list(APPEND Source_Files "GameTechRenderer.cpp")
	list(APPEND Header_Files "NetworkedGame.h")
	list(APPEND Source_Files "NetworkedGame.cpp")
	list(APPEND Header_Files "NetworkedPlayer.h")
	list(APPEND Source_Files "NetworkedPlayer.cpp")
else()
	list(APPEND Header_Files "PlayStation4Renderer.h")
	list(APPEND Source_Files "PlayStation4Renderer.cpp")
endif()

source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

if(USE_VULKAN)
    set(ALL_FILES 
        ${Header_Files}
        ${Source_Files}
        ${SHADER_FILES}
    )

foreach (file ${SHADER_FILES})
    get_filename_component(file_name ${file} NAME)
	get_filename_component(file_ext ${file} EXT)
	
	if(file_ext STREQUAL ".h" OR file_ext STREQUAL ".cpp")
		continue()
	endif()
		
	if( file_ext STREQUAL  ".vert" OR
		file_ext STREQUAL  ".frag" OR
		file_ext STREQUAL  ".comp" OR
		file_ext STREQUAL  ".geom" OR
		file_ext STREQUAL  ".tesc" OR
		file_ext STREQUAL  ".tese" OR
		file_ext STREQUAL  ".rgen" OR
		file_ext STREQUAL  ".rint" OR
		file_ext STREQUAL  ".rahit" OR
		file_ext STREQUAL  ".rchit" OR
		file_ext STREQUAL  ".rmiss" OR
		file_ext STREQUAL  ".rcall" OR
		file_ext STREQUAL  ".task" OR
		file_ext STREQUAL  ".mesh"
	)
		message("Adding custom command to ${file}")
		get_filename_component(file_dir ${file} ABSOLUTE)
        set(SPIRV_OUTPUT ${file_name}.spv)
		set(SPIRV_ABS_INPUT ${file_dir})
		set(SPIRV_ABS_OUTPUT ${file_dir}.spv)
		
		add_custom_command(
			OUTPUT ${SPIRV_ABS_OUTPUT}
			
			COMMENT "Compiling GLSL shader:"
			COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} -V  ${SPIRV_ABS_INPUT} -o ${SPIRV_ABS_OUTPUT}
            DEPENDS ${file}
			VERBATIM
		)
        list(APPEND SPIRV_BINARY_FILES ${SPIRV_OUTPUT})   
	endif()
endforeach()

endif()

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME}  ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE CSC8503)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)

################################################################################
# Compile definitions
################################################################################
if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "UNICODE;"
        "_UNICODE" 
        "WIN32_LEAN_AND_MEAN"
        "_WINSOCKAPI_"   
        "_WINSOCK2API_"
        "_WINSOCK_DEPRECATED_NO_WARNINGS"
    )
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE
    <vector>
    <map>
    <stack>
    <string>
    <list>
    <thread>
    <atomic>
    <functional>
    <iostream>
    <set>
    "../NCLCoreClasses/Vector2.h"
    "../NCLCoreClasses/Vector3.h"
    "../NCLCoreClasses/Vector4.h"
    "../NCLCoreClasses/Quaternion.h"
    "../NCLCoreClasses/Plane.h"
    "../NCLCoreClasses/Matrix2.h"
    "../NCLCoreClasses/Matrix3.h"
    "../NCLCoreClasses/Matrix4.h"
    
)
 
if(USE_VULKAN)
target_precompile_headers(${PROJECT_NAME} PRIVATE
    "../VulkanRendering/SmartTypes.h"

    "../VulkanRendering/VulkanRenderer.h"
    "../VulkanRendering/VulkanShader.h"
    "../VulkanRendering/VulkanShaderBuilder.h"
    "../VulkanRendering/VulkanTexture.h"
    "../VulkanRendering/VulkanMesh.h"
    "../VulkanRendering/VulkanPipelineBuilder.h"
    "../VulkanRendering/VulkanDynamicRenderBuilder.h"

    "../VulkanRendering/VulkanDescriptorSetLayoutBuilder.h"
    "../VulkanRendering/VulkanRenderPassBuilder.h"

    "../VulkanRendering/VulkanCompute.h"
    "../VulkanRendering/VulkanComputePipelineBuilder.h"
)
endif()

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
endif()

################################################################################
# Dependencies
################################################################################
if(MSVC)
    target_link_libraries(${PROJECT_NAME} LINK_PUBLIC  "Winmm.lib")
endif()

include_directories("../NCLCoreClasses/")
include_directories("../CSC8503CoreClasses/")

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC NCLCoreClasses)
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC CSC8503CoreClasses)

if(WIN32)
	include_directories("../OpenGLRendering/")
	target_link_libraries(${PROJECT_NAME} LINK_PUBLIC OpenGLRendering)
else()
	include_directories("../PlayStation4Rendering/")
	target_link_libraries(${PROJECT_NAME} LINK_PUBLIC PlayStation4Rendering)

endif()

if(USE_VULKAN)
include_directories("../VulkanRendering/")
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC VulkanRendering)


add_custom_target(
    SHADER_FILES
    DEPENDS ON ${SPIRV_BINARY_FILES}
)

add_dependencies(${PROJECT_NAME} SHADER_FILES)

endif()

################################################################################
# OpenAL config
################################################################################
include_directories("../audioLib/")
link_directories(OPENAL_LIBRARY OPENAL_INCLUDE_DIR)


target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${CMAKE_SOURCE_DIR}/builtLibs/debug/OpenAL/OpenAL32.lib)
#target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${CMAKE_SOURCE_DIR}/builtLibs/debug/OpenAL/OpenAL32.dll)
