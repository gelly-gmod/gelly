# shameless copy from gelly

function(find_hlsl_files directory output_var)
    file(GLOB_RECURSE files "${directory}/*.hlsl")
    foreach (file ${files})
        if (${file} MATCHES ".+embed")
            continue()
        endif ()
        # Strip last character
        list(APPEND file_names ${file})
    endforeach ()
    set(${output_var} ${file_names} PARENT_SCOPE)
endfunction()