function(target_import_src target_name)
    target_add_source_recurse(${target_name} ${CMAKE_CURRENT_SOURCE_DIR}/src/)
    add_and_install_headers_recurse(${target_name} ${CMAKE_CURRENT_SOURCE_DIR}/src/)
endfunction()