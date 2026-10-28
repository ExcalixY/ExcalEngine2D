# Copies a game's asset folder next to its executable as Assets/ after every build
function(excal_add_assets target source_dir)
  add_custom_command(TARGET ${target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
      ${source_dir} $<TARGET_FILE_DIR:${target}>/Assets
    COMMENT "Copying assets for ${target}"
    VERBATIM
  )
endfunction()
