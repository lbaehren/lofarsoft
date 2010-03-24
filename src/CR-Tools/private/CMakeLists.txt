
foreach (_user
    baehren
    deaton
    falcke
    horneffer
    link
    nigl
    terveen
    welles
    )
  ## generate local variables
  string (TOUPPER ${_user} _name)
  set (_option "CR_PRIVATE_${_name}")
  ## option to build private code
  option (${_option} "Include building private code from user ${_user}?" NO )
  ## add sub-directory to the project
  if (${_option})
    message (STATUS "[CR-Tools] Adding directory private/${_user} ...")
    ## assemble list of source files
    file (GLOB cr_${_user}_sources ${_user}/*.cc ${_user}/*.cpp)
    ## add sub-directory
    add_subdirectory (${_user})
  endif (${_option})
endforeach (_user)
