
file (GLOB PRESTO_HEADER_FILES *.h)

if (PRESTO_HEADER_FILES)

  message (STATUS "[Presto] PRESTO_HEADER_FILES = ${PRESTO_HEADER_FILES}")
  
  install (FILES
    ${PRESTO_HEADER_FILES}
    DESTINATION include
    )
  
else (PRESTO_HEADER_FILES)

  message (STATUS "[Presto] No header files found to install!")

endif (PRESTO_HEADER_FILES)
