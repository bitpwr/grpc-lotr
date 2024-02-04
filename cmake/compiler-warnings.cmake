function(set_compiler_warnings)

    set(GCC_WARNINGS
        -Werror
        -Wall
        -Wcast-align
        -Wconversion
        -Wdouble-promotion
        -Wduplicated-branches
        -Wduplicated-cond
        -Wextra
        -Wformat=2
        -Wimplicit-fallthrough
        -Wlogical-op
        -Wmisleading-indentation
        -Wno-psabi
        -Wnon-virtual-dtor
        # -Wnull-dereference - boost::program_options problems...
        -Wold-style-cast
        -Woverloaded-virtual
        -Wpedantic
        -Wshadow
        -Wsign-conversion
        -Wunused
        -Wuseless-cast
    )

    set(CLANG_WARNINGS
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unused-local-typedefs
        -Wno-unused-parameter
        -Walloca
        -Wassign-enum
        -Wbad-function-cast
        -Wbitfield-enum-conversion
        -Wbridge-cast
        -Wc++-compat
        -Wcast-align
        -Wcast-qual
        -Wcomma
        -Wconditional-uninitialized
        -Wextra-semi
        -Wheader-hygiene
        -Wimplicit-fallthrough
        -Winconsistent-missing-override
        -Winconsistent-missing-destructor-override
        -Wloop-analysis
        -Wzero-as-null-pointer-constant
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(PROJECT_WARNINGS_CXX ${GCC_WARNINGS})
    elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        set(PROJECT_WARNINGS_CXX ${CLANG_WARNINGS})
    else()
        message(WARNING "No compiler warnings set for CXX compiler: '${CMAKE_CXX_COMPILER_ID}'")
    endif()

    add_compile_options(${PROJECT_WARNINGS_CXX})
endfunction()
