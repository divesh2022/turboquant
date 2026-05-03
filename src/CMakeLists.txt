add_subdirectory(core)
add_subdirectory(transforms)

add_library(turboquant_core STATIC "")
target_include_directories(turboquant_core PUBLIC /include)

# Should be something like:
target_sources(turboquant_core PRIVATE 
    core/qjl.cpp
    transforms/polar.cpp
)
