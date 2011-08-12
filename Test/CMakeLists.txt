cmake_minimum_required (VERSION 2.8) 
project (CoreTest) 

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS thread-mt signals-mt )

include_directories ( ${MicroFramework.Core_SOURCE_DIR} ) 

file( GLOB_RECURSE CoreTestSources "Core/*.cpp" "Core/*.h" )

add_executable ( CoreTest ${CoreTestSources} )
target_link_libraries ( CoreTest MicroFramework.Core ${Boost_LIBRARIES}  pugixml )
