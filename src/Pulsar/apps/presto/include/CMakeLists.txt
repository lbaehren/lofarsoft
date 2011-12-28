
file (GLOB PRESTO_HEADER_FILES *.h)

if (PRESTO_HEADER_FILES)

  ## Installation instructions for header files
  install (FILES
    ${PRESTO_HEADER_FILES}
    DESTINATION include
    )
  
else (PRESTO_HEADER_FILES)

  message (STATUS "[Presto] No header files found to install!")

endif (PRESTO_HEADER_FILES)
