macro(set_default_options target)
    target_compile_options(${target} PRIVATE
		-Wall
		-Wextra
		-Wpedantic
		-Werror
    )
endmacro()

macro(set_sanitiser_options target)
	target_compile_options(${target} PRIVATE
		-fsanitize=address,undefined,float-divide-by-zero,unsigned-integer-overflow,implicit-conversion,local-bounds
	)
	target_link_options(${target} PRIVATE
		-fsanitize=address,undefined,float-divide-by-zero,unsigned-integer-overflow,implicit-conversion,local-bounds
	)
endmacro()
