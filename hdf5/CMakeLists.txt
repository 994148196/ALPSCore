cmake_minimum_required (VERSION 2.8)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/../common/cmake)

project (alps-hdf5)

set(DOXYFILE_SOURCE_DIR "./include")
include(UseDoxygen)

# Boost
#set(Boost_USE_STATIC_LIBS        ON)
#set(Boost_USE_STATIC_RUNTIME    OFF)
find_package (Boost COMPONENTS filesystem system)
    message(STATUS "Boost includes: " ${Boost_INCLUDE_DIRS} )
    message(STATUS "Boost libs: " ${Boost_LIBRARIES} )
find_package (HDF5)
    message(STATUS "HDF5 includes: " ${HDF5_INCLUDE_DIRS} )
    message(STATUS "HDF5 libs: " ${HDF5_LIBRARIES} )

# LINK_ALL - link for libraries
# LINK_TEST - link for tests
set (LINK_ALL ${Boost_LIBRARIES} ${HDF5_LIBRARIES})
set (LINK_TEST )

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${HDF5_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}/include
)

#configure_file("${PROJECT_SOURCE_DIR}/include/config.h.in" "${CMAKE_BINARY_DIR}/include/alps/config.h")
add_subdirectory(src)

option(Testing "Enable testing" ON)
if (Testing)
    message(STATUS "Building tests")
    enable_testing()
    include(UseGtest)
    set (LINK_TEST gtest_main)
    add_subdirectory(${gtest_ROOT} ${PROJECT_BINARY_DIR}/gtest)
    include_directories(${gtest_INCLUDE_DIR})
    add_subdirectory(test)
endif (Testing)

# installation
install(DIRECTORY include DESTINATION .
          FILES_MATCHING PATTERN "*.hpp" PATTERN "*.hxx"
        )

# Generate pkg-config file
configure_file("${PROJECT_SOURCE_DIR}/${PROJECT_NAME}.pc.in" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc")
configure_file("${CMAKE_SOURCE_DIR}/../common/cmake/FindALPSModule.cmake.in" "${CMAKE_BINARY_DIR}/Find${PROJECT_NAME}.cmake" @ONLY)
install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION "lib/pkgconfig")
install(FILES "${CMAKE_BINARY_DIR}/Find${PROJECT_NAME}.cmake" DESTINATION "share/cmake/Modules")
