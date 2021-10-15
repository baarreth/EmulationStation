project("clear_bt_cache")

set (CMAKE_INSTALL_BINDIR /usr/local/bin)

set(clear_bt_cache_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/clear_bt_cache.c
)

set(CMAKE_C_FLAGS -Wno-trigraphs)

add_executable(clear_bt_cache ${clear_bt_cache_SOURCES})

# install(TARGETS clear_bt_cache RUNTIME DESTINATION bin)
install (CODE "execute_process(COMMAND cp clear_bt_cache ${CMAKE_INSTALL_BINDIR}/)")
install (CODE "execute_process(COMMAND chown root:root ${CMAKE_INSTALL_BINDIR}/clear_bt_cache)")
install (CODE "execute_process(COMMAND chmod 4755 ${CMAKE_INSTALL_BINDIR}/clear_bt_cache)")
