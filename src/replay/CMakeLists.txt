if(NOT X11_FOUND)
    message(FATAL_ERROR "XPILOT_BUILD_REPLAY is ON, but X11 was not found.")
endif()

file(GLOB REPLAY_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(xpilot-ng-cpp-replay ${REPLAY_SOURCES})

target_include_directories(xpilot-ng-cpp-replay
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/common
        ${CMAKE_SOURCE_DIR}/src/client
        ${CMAKE_SOURCE_DIR}/src/client/items
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${X11_INCLUDE_DIR}
)

target_compile_definitions(xpilot-ng-cpp-replay
    PRIVATE
        HAVE_CONFIG_H=1
        CONF_DATADIR="${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}/"
)

target_link_libraries(xpilot-ng-cpp-replay
    PRIVATE
        xpcommon
        ZLIB::ZLIB
        ${X11_LIBRARIES}
)

install(TARGETS xpilot-ng-cpp-replay
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
