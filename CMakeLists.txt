cmake_minimum_required(VERSION 3.21)

project(main
        VERSION 0.0.1
        DESCRIPTION ""
        LANGUAGES C)

message(STATUS "Compiler being used: ${CMAKE_C_COMPILER}")

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/src/main.c)

set(main_SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.c
)

set(main_HEADERS
)

add_executable(main ${main_SOURCES})
target_include_directories(main PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(main PRIVATE /usr/local/include)

# Check if the system uses 64-bit libraries
get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)

#Set the library suffix based on whether it's 64-bit or not
if ("${LIB64}" STREQUAL "TRUE")
   set(LIBSUFFIX 64)
else()
    set(LIBSUFFIX "")
endif()

target_link_directories(main PRIVATE /usr/local/lib${LIBSUFFIX})
target_link_options(main PRIVATE ${INSTRUMENTATION_FLAGS_LIST})


message("C Compiler: ${CMAKE_C_COMPILER}")
get_filename_component(COMPILER_NAME "${CMAKE_C_COMPILER}" NAME_WE)
message("COMPILER_NAME: ${COMPILER_NAME}")

function(split_string_into_list _input_string _output_list)
    string(REGEX REPLACE "[ ]+" ";" _split_list "${_input_string}")
    set(${_output_list} ${_split_list} PARENT_SCOPE)
endfunction()

file(STRINGS "${CMAKE_SOURCE_DIR}/.flags/${COMPILER_NAME}/analyzer_flags.txt" ANALYZER_FLAGS_STRING)
split_string_into_list("${ANALYZER_FLAGS_STRING}" ANALYZER_FLAGS_LIST)

file(STRINGS "${CMAKE_SOURCE_DIR}/.flags/${COMPILER_NAME}/debug_flags.txt" DEBUG_FLAGS_STRING)
split_string_into_list("${DEBUG_FLAGS_STRING}" DEBUG_FLAGS_LIST)

file(STRINGS "${CMAKE_SOURCE_DIR}/.flags/${COMPILER_NAME}/instrumentation_flags.txt" INSTRUMENTATION_FLAGS_STRING)
split_string_into_list("${INSTRUMENTATION_FLAGS_STRING}" INSTRUMENTATION_FLAGS_LIST)

file(STRINGS "${CMAKE_SOURCE_DIR}/.flags/${COMPILER_NAME}/optimization_flags.txt" OPTIMIZATION_FLAGS_STRING)
split_string_into_list("${OPTIMIZATION_FLAGS_STRING}" OPTIMIZATION_FLAGS_LIST)

file(STRINGS "${CMAKE_SOURCE_DIR}/.flags/${COMPILER_NAME}/warning_flags.txt" WARNING_FLAGS_STRING)
split_string_into_list("${WARNING_FLAGS_STRING}" WARNING_FLAGS_LIST)

if(SANITIZER_address STREQUAL "ON")
    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/.flags/${COMPILER_NAME}/address_sanitizer_flags.txt" ADDRESS_SANITIZER_FLAGS_STRING)
    string(REPLACE " " ";" ADDRESS_SANITIZER_FLAGS_LIST "${ADDRESS_SANITIZER_FLAGS_STRING}")
    target_compile_options(main PRIVATE ${ADDRESS_SANITIZER_FLAGS_LIST})
    target_link_options(main PRIVATE ${ADDRESS_SANITIZER_FLAGS_LIST})
endif()

if(SANITIZER_cfi STREQUAL "ON")
    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/.flags//cfi_sanitizer_flags.txt" CFI_SANITIZER_FLAGS_STRING)
    string(REPLACE " " ";" CFI_SANITIZER_FLAGS_LIST "${CFI_SANITIZER_FLAGS_STRING}")
    target_compile_options(main PRIVATE ${CFI_SANITIZER_FLAGS_LIST})
    target_link_options(main PRIVATE ${CFI_SANITIZER_FLAGS_LIST})
endif()

if(SANITIZER_dataflow STREQUAL "ON")
    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/.flags//dataflow_sanitizer_flags.txt" DATAFLOW_SANITIZER_FLAGS_STRING)
    string(REPLACE " " ";" DATAFLOW_SANITIZER_FLAGS_LIST "${DATAFLOW_SANITIZER_FLAGS_STRING}")
    target_compile_options(main PRIVATE ${DATAFLOW_SANITIZER_FLAGS_LIST})
    target_link_options(main PRIVATE ${DATAFLOW_SANITIZER_FLAGS_LIST})
endif()

if(SANITIZER_hwaddress STREQUAL "ON")
    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/.flags//hwaddress_sanitizer_flags.txt" HWADDRESS_SANITIZER_FLAGS_STRING)
    string(REPLACE " " ";" HWADDRESS_SANITIZER_FLAGS_LIST "${HWADDRESS_SANITIZER_FLAGS_STRING}")
    target_compile_options(main PRIVATE ${HWADDRESS_SANITIZER_FLAGS_LIST})
    target_link_options(main PRIVATE ${HWADDRESS_SANITIZER_FLAGS_LIST})
endif()

if(SANITIZER_memory STREQUAL "ON")
    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/.flags//memory_sanitizer_flags.txt" MEMORY_SANITIZER_FLAGS_STRING)
    string(REPLACE " " ";" MEMORY_SANITIZER_FLAGS_LIST "${MEMORY_SANITIZER_FLAGS_STRING}")
    target_compile_options(main PRIVATE ${MEMORY_SANITIZER_FLAGS_LIST})
    target_link_options(main PRIVATE ${MEMORY_SANITIZER_FLAGS_LIST})
endif()

if(SANITIZER_pointer_overflow STREQUAL "ON")
    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/.flags//pointer_overflow_sanitizer_flags.txt" POINTER_OVERFLOW_SANITIZER_FLAGS_STRING)
    string(REPLACE " " ";" POINTER_OVERFLOW_SANITIZER_FLAGS_LIST "${POINTER_OVERFLOW_SANITIZER_FLAGS_STRING}")
    target_compile_options(main PRIVATE ${POINTER_OVERFLOW_SANITIZER_FLAGS_LIST})
    target_link_options(main PRIVATE ${POINTER_OVERFLOW_SANITIZER_FLAGS_LIST})
endif()

if(SANITIZER_safe_stack STREQUAL "ON")
    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/.flags//safe_stack_sanitizer_flags.txt" SAFE_STACK_SANITIZER_FLAGS_STRING)
    string(REPLACE " " ";" SAFE_STACK_SANITIZER_FLAGS_LIST "${SAFE_STACK_SANITIZER_FLAGS_STRING}")
    target_compile_options(main PRIVATE ${SAFE_STACK_SANITIZER_FLAGS_LIST})
    target_link_options(main PRIVATE ${SAFE_STACK_SANITIZER_FLAGS_LIST})
endif()

if(SANITIZER_thread STREQUAL "ON")
    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/.flags//thread_sanitizer_flags.txt" THREAD_SANITIZER_FLAGS_STRING)
    string(REPLACE " " ";" THREAD_SANITIZER_FLAGS_LIST "${THREAD_SANITIZER_FLAGS_STRING}")
    target_compile_options(main PRIVATE ${THREAD_SANITIZER_FLAGS_LIST})
    target_link_options(main PRIVATE ${THREAD_SANITIZER_FLAGS_LIST})
endif()

if(SANITIZER_undefined STREQUAL "ON")
    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/.flags/${COMPILER_NAME}/undefined_sanitizer_flags.txt" UNDEFINED_SANITIZER_FLAGS_STRING)
    string(REPLACE " " ";" UNDEFINED_SANITIZER_FLAGS_LIST "${UNDEFINED_SANITIZER_FLAGS_STRING}")
    target_compile_options(main PRIVATE ${UNDEFINED_SANITIZER_FLAGS_LIST})
    target_link_options(main PRIVATE ${UNDEFINED_SANITIZER_FLAGS_LIST})
endif()

set(STANDARD_FLAGS
    -D_POSIX_C_SOURCE=200809L
    -D_XOPEN_SOURCE=700
    -D_GNU_SOURCE
    -D_DARWIN_C_SOURCE
    -D__BSD_VISIBLE
    -Werror
)

target_link_directories(main PRIVATE /usr/local/lib${LIBSUFFIX})
target_link_options(main PRIVATE ${INSTRUMENTATION_FLAGS_LIST})

# Set compiler flags for the target main
target_compile_options(main PRIVATE
    ${STANDARD_FLAGS}
    ${ANALYZER_FLAGS_LIST}
    ${DEBUG_FLAGS_LIST}
    ${INSTRUMENTATION_FLAGS_LIST}
    ${OPTIMIZATION_FLAGS_LIST}
    ${WARNING_FLAGS_LIST}
)
# Add target_link_libraries for main

if (NOT DEFINED CLANG_FORMAT_NAME)
    set(CLANG_FORMAT_NAME "clang-format")
endif()

if (NOT DEFINED CLANG_TIDY_NAME)
    set(CLANG_TIDY_NAME "clang-tidy")
endif()

if (NOT DEFINED CPPCHECK_NAME)
    set(CPPCHECK_NAME "cppcheck")
endif()

find_program(CLANG_FORMAT NAMES ${CLANG_FORMAT_NAME} REQUIRED)
find_program(CLANG_TIDY NAMES ${CLANG_TIDY_NAME} REQUIRED)
find_program(CPPCHECK NAMES ${CPPCHECK_NAME} REQUIRED)

add_custom_target(format
    COMMAND ${CLANG_FORMAT} --style=file -i ${SOURCES} ${HEADERS}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-format"
)

add_dependencies(main format)

add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CLANG_TIDY} ${SOURCES} ${HEADERS} -quiet --warnings-as-errors='*' -checks=*,-llvmlibc-restrict-system-libc-headers,-altera-struct-pack-align,-readability-identifier-length,-altera-unroll-loops,-cppcoreguidelines-init-variables,-cert-err33-c,-modernize-macro-to-enum,-bugprone-easily-swappable-parameters,-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling,-altera-id-dependent-backward-branch,-concurrency-mt-unsafe,-misc-unused-parameters,-hicpp-signed-bitwise,-google-readability-todo,-cert-msc30-c,-cert-msc50-cpp,-readability-function-cognitive-complexity,-clang-analyzer-security.insecureAPI.strcpy,-cert-env33-c,-android-cloexec-accept,-clang-analyzer-security.insecureAPI.rand,-misc-include-cleaner,-llvm-header-guard -- ${STANDARD_FLAGS} -I${CMAKE_SOURCE_DIR}/include -I/usr/local/include
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-tidy"
)

if (CMAKE_C_COMPILER MATCHES ".*/clang.*")
    # Add a custom target for clang --analyze
    add_custom_command(
        TARGET main POST_BUILD
        COMMAND ${CMAKE_C_COMPILER} --analyzer-output text --analyze -Xclang -analyzer-checker=core --analyze -Xclang -analyzer-checker=deadcode -Xclang -analyzer-checker=security -Xclang -analyzer-disable-checker=security.insecureAPI.DeprecatedOrUnsafeBufferHandling -Xclang -analyzer-checker=unix -Xclang -analyzer-checker=unix ${CMAKE_C_FLAGS} ${STANDARD_FLAGS} -I${CMAKE_SOURCE_DIR}/include -I/usr/local/include ${SOURCES} ${HEADERS}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running clang --analyze"
    )

    # Add a custom command to delete .gch files after the analysis
    add_custom_command(
        TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_SOURCE_DIR}/include/*.gch
        COMMENT "Removing .gch files"
    )
endif ()

add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CPPCHECK} --error-exitcode=1 --force --quiet --inline-suppr --library=posix --enable=all --suppress=missingIncludeSystem --suppress=unusedFunction --suppress=unmatchedSuppression --suppress=checkersReport -I${CMAKE_SOURCE_DIR}/include -I/usr/local/include ${SOURCES} ${HEADERS}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running cppcheck"
)

