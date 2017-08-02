# add_test(NAME <name> COMMAND <command> [<arg>...]
#          [CONFIGURATIONS <config>...]
#          [WORKING_DIRECTORY <dir>])

set(testing_dir ${CMAKE_BINARY_DIR}/tests)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${testing_dir})

set(pkg_extracted_dir "${testing_dir}/${PKG_STAGE_DIR}")
if(WIN32)
  set(extract_opts "x;${PACKAGE_OUTPUT_FILE}")
  set(extract_cmd "${ZIP_EXECUTABLE}")
  set(my_ext ".bat")
else()
  set(extract_opts "--target;${pkg_extracted_dir}")
  set(extract_cmd "${PACKAGE_OUTPUT_FILE}")
  set(my_ext ".sh")
endif()

add_test(
  NAME Tu_install_package
  COMMAND ${extract_cmd}
  ${extract_opts}
  WORKING_DIRECTORY ${testing_dir}
  )

add_test(
  NAME Tu_selftester
  COMMAND ${pkg_extracted_dir}/tools/selftester${my_ext}
  WORKING_DIRECTORY ${pkg_extracted_dir}
  )

set_tests_properties ( Tu_selftester PROPERTIES DEPENDS Tu_install_package)

add_test(
  NAME Tu_uninstall_otb
  COMMAND ${pkg_extracted_dir}/uninstall_otb${my_ext}
  WORKING_DIRECTORY ${testing_dir}
  )