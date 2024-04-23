macro(wfc_set_defaults target)
    target_compile_options(${target} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
    )
    set_target_properties(${target} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )
    target_include_directories(${target} PRIVATE src)
endmacro()

macro(wfc_set_sanitiser_options target)
    target_compile_options(${target} PRIVATE
        -fsanitize=address,undefined,float-divide-by-zero,unsigned-integer-overflow,implicit-conversion,local-bounds
    )
    target_link_options(${target} PRIVATE
        -fsanitize=address,undefined,float-divide-by-zero,unsigned-integer-overflow,implicit-conversion,local-bounds
    )
endmacro()
