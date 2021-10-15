project("reboot_overclock")

set (CMAKE_INSTALL_BINDIR /usr/local/bin)

set(reboot_overclock_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/reboot_overclock.c
)

# set(CMAKE_C_FLAGS -Wno-trigraphs)

add_executable(reboot_overclock ${reboot_overclock_SOURCES})

# install(TARGETS reboot_overclock RUNTIME DESTINATION bin)
install (CODE "execute_process(COMMAND cp reboot_overclock ${CMAKE_INSTALL_BINDIR}/)")
install (CODE "execute_process(COMMAND chown root:root ${CMAKE_INSTALL_BINDIR}/reboot_overclock)")
install (CODE "execute_process(COMMAND chmod 4755 ${CMAKE_INSTALL_BINDIR}/reboot_overclock)")
