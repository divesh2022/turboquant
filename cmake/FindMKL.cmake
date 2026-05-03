// give code for FindMKL.cmake keep it modular , object oriented and use C++17 features and do execption handelling

# - Find Intel MKL (Math Kernel Library)
#
# This module defines the following variables and imported targets:
#  MKL_FOUND        - True if MKL is found
#  MKL_INCLUDE_DIRS - Directory containing MKL headers
#  MKL_LIBRARIES    - The libraries to link against
#
# Target:
#  MKL::MKL

include(FindPackageHandleStandardArgs)

# 1. Search for include directory
find_path(MKL_INCLUDE_DIR
    NAMES mkl.h
    PATHS
        $ENV{MKLROOT}/include
        /opt/intel/oneapi/mkl/latest/include
        /opt/intel/mkl/include
        "C:/Program Files (x86)/Intel/oneAPI/mkl/latest/include"
    DOC "Path to MKL include directory"
)

# 2. Determine target architecture and find the correct libraries
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(MKL_ARCH_DIR "intel64")
else()
    set(MKL_ARCH_DIR "ia32")
endif()

# Find the main MKL libraries depending on the OS/Platform
find_library(MKL_CORE_LIBRARY
    NAMES mkl_rt mkl_intel_lp64 mkl_intel_thread
    PATHS
        $ENV{MKLROOT}/lib/${MKL_ARCH_DIR}
        /opt/intel/oneapi/mkl/latest/lib/${MKL_ARCH_DIR}
        /opt/intel/mkl/lib/${MKL_ARCH_DIR}
        "C:/Program Files (x86)/Intel/oneAPI/mkl/latest/lib"
    DOC "MKL Core Library"
)

# Set the include and library variables
set(MKL_INCLUDE_DIRS ${MKL_INCLUDE_DIR})
set(MKL_LIBRARIES ${MKL_CORE_LIBRARY})

# 3. Handle standard arguments (checks for missing variables)
find_package_handle_standard_args(MKL
    DEFAULT_MSG
    MKL_CORE_LIBRARY
    MKL_INCLUDE_DIR
)

# 4. Create Modern CMake Imported Target
if(MKL_FOUND AND NOT TARGET MKL::MKL)
    add_library(MKL::MKL INTERFACE IMPORTED)
    
    # Configure modular target properties
    set_target_properties(MKL::MKL PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${MKL_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${MKL_LIBRARIES}"
        INTERFACE_COMPILE_DEFINITIONS "MKL_ILP64"
    )
    
    message(STATUS "[CMake] MKL Target successfully configured: MKL::MKL")
endif()

# Mark internal variables as advanced to keep GUI clean
mark_as_advanced(MKL_INCLUDE_DIR MKL_CORE_LIBRARY)