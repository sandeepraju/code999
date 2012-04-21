set(QT_USE_QTNETWORK TRUE)

set(moviemanager_SRCS
   moviemanager.cpp
   main.cpp
   imdb.cpp
   qdownloader.cpp
 )

FIND_PACKAGE(QJSON)

############## KDE, Nepomuk, Soprano ######
find_package(KDE4 REQUIRED)
find_package(Nepomuk REQUIRED)
find_package(NepomukExtras)
include(MacroLibrary)
include(SopranoAddOntology)
include (KDE4Defaults)
##########################################

kde4_add_executable(moviemanager ${moviemanager_SRCS})

target_link_libraries(moviemanager ${KDE4_KDEUI_LIBS} )
target_link_libraries(moviemanager ${QT_QTWEBKIT_LIBRARY})
target_link_libraries(moviemanager ${QJSON_LIBRARIES})

install(TARGETS moviemanager ${INSTALL_TARGETS_DEFAULT_ARGS} )

########### link libraries #########
target_link_libraries( moviemanager
  nepomukutils
  nepomukextras
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
  ${NEPOMUK_LIBRARIES}
  ${NEPOMUK_QUERY_LIBRARIES}
  ${SOPRANO_LIBRARIES}
  ${NEPOMUKEXTRAS_INCLUDE_DIR}
  )

#########################################

