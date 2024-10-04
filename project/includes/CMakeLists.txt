cmake_minimum_required(VERSION 3.0)
set(CMAKE_CURRENT_BINARY_DIR, ./build)
project(lab3)

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

find_package(BISON)
find_package(FLEX)
if (${BISON_FOUND})
  message("Bison found")
  if (${FLEX_FOUND})
    message("Flex found")
    BISON_TARGET(parser ${lab3_SOURCE_DIR}/parser/struct.y ${lab3_SOURCE_DIR}/src/parser/parser.c
                 COMPILE_FLAGS -Wcex
                 DEFINES_FILE ${lab3_SOURCE_DIR}/includes/parser/parser.h
                 VERBOSE 
                 REPORT_FILE ${lab3_SOURCE_DIR}/docs/bison.txt)
    FLEX_TARGET(lexer ${lab3_SOURCE_DIR}/lexer/struct.l ${lab3_SOURCE_DIR}/src/lexer/lexer.c
                DEFINES_FILE ${lab3_SOURCE_DIR}/includes/lexer/lexer.h)
    ADD_FLEX_BISON_DEPENDENCY(lexer parser)
  else()
    message("Flex not found!")
  endif()
else ()
  message("Bison not found!")
endif()

set(SOURCES
  ${lab3_SOURCE_DIR}/src/main.c
  ${lab3_SOURCE_DIR}/src/api/ast_api.c
  ${lab3_SOURCE_DIR}/src/api/ast_riscv_printer.cpp
  ${lab3_SOURCE_DIR}/src/api/ast_text_printers.c
  ${lab3_SOURCE_DIR}/src/ast/ast.c
  ${lab3_SOURCE_DIR}/src/ast/var_dict.cpp
  ${lab3_SOURCE_DIR}/src/parser/parser_res.c
  ${BISON_parser_OUTPUT_SOURCE}
  ${FLEX_lexer_OUTPUTS}
)

set(INCLUDES 
  ${lab3_SOURCE_DIR}/includes/
  ${lab3_SOURCE_DIR}/includes/api/
  ${lab3_SOURCE_DIR}/includes/ast/
  ${lab3_SOURCE_DIR}/includes/parser/
  ${lab3_SOURCE_DIR}/includes/lexer/
)

add_library(db ${SOURCES})
target_include_directories(db PRIVATE ${INCLUDES})

add_executable(main src/main.c)
target_link_libraries(main PRIVATE db)
target_include_directories(main PRIVATE ${INCLUDES})