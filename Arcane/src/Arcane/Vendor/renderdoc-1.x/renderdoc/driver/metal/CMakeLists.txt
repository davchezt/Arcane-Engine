set(sources
    metal_dispatch_table_bridge.mm
    metal_dispatch_table_bridge.h
    metal_hook_bridge.mm
    metal_hook.cpp
    metal_hook.h
    metal_device.cpp
    metal_device.h
    metal_device_bridge.mm
    metal_resources.cpp
    metal_resources.h
    metal_types_bridge.mm
    metal_types_bridge.h
    metal_types.cpp
    metal_types.h
    metal_library.cpp
    metal_library.h
    metal_library_bridge.mm
    metal_function.cpp
    metal_function.h
    metal_function_bridge.mm
    metal_common.h
    metal_command_queue.cpp
    metal_command_queue.h
    metal_command_queue_bridge.mm
    metal_command_buffer.cpp
    metal_command_buffer.h
    metal_command_buffer_bridge.mm
    metal_core.cpp
    metal_core.h
    metal_manager.cpp
    metal_manager.h
    metal_init_state.cpp
    metal_helpers_bridge.h
    metal_helpers_bridge.mm
    official/metal-cpp.h
    official/metal-cpp.cpp)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
add_library(rdoc_metal OBJECT ${sources})
target_compile_definitions(rdoc_metal ${RDOC_DEFINITIONS})
target_include_directories(rdoc_metal ${RDOC_INCLUDES})
