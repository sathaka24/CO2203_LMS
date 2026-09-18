# ---- Core library: all project code except main.cpp ----
add_library(lms_core STATIC)

# Same as -Iinclude in the Makefile.
# PUBLIC means anything linking lms_core also gets this include path.
target_include_directories(lms_core PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Same as -Wall in the Makefile
if(MSVC)
    target_compile_options(lms_core PUBLIC /W4)
else()
    target_compile_options(lms_core PUBLIC -Wall)
endif()

# Each module adds its own .cpp files to lms_core
add_subdirectory(domain)
add_subdirectory(exception)
add_subdirectory(persistence)
add_subdirectory(scheduling)
add_subdirectory(attendance)
add_subdirectory(third_party)

# ---- Executable (same name as the Makefile's bin/registration) ----
add_executable(registration main.cpp)
target_link_libraries(registration PRIVATE lms_core)

# "make run" equivalent:  cmake --build build --target run
add_custom_target(run
    COMMAND registration
    DEPENDS registration
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    USES_TERMINAL
)