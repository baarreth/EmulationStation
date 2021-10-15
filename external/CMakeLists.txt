# set up the external libraries that aren't popular enough to be found on most 
# package managers are included with the project (in the 'external' folder)

add_subdirectory("nanosvg")
add_subdirectory("pugixml")


# including a hack to clean the bluetooth cache

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_subdirectory("bluetoothhack")
endif()


