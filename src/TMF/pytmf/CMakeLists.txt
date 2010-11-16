include_directories (${BOOST_INCLUDES} ${PYTHON_INCLUDES})

add_library (pytmf MODULE pytmf.cc)

target_link_libraries (pytmf
    tmf
    ${BOOST_LIBRARIES}
    ${PYTHON_LIBRARIES}
    )

if (APPLE)
    set_target_properties(pytmf
        PROPERTIES
        PREFIX ""
        SUFFIX ".so"
        COMPILE_FLAGS "-DPYTHON -fpermissive -O2"
        LINK_FLAGS "-fPIC -flat_namespace"
        )
else (APPLE)
    set_target_properties(pytmf
        PROPERTIES
        PREFIX ""
        SUFFIX ".so"
        COMPILE_FLAGS "-DPYTHON -O2"
        LINK_FLAGS "-fPIC -shared -Wl"
        )
endif (APPLE)

install (TARGETS pytmf
    LIBRARY DESTINATION lib/python
    )

