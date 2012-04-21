set(moviemanager_SRCS
   moviemanager.cpp
   main.cpp
 )

kde4_add_executable(moviemanager ${moviemanager_SRCS})

target_link_libraries(moviemanager ${KDE4_KDEUI_LIBS} )

install(TARGETS moviemanager ${INSTALL_TARGETS_DEFAULT_ARGS} )

