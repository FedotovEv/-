cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(SpreadSheetFrontend)

set(CMAKE_CXX_STANDARD 17)

include_directories(
  ${ANTLR4_INCLUDE_DIRS}
  ${ANTLR_FormulaParser_OUTPUT_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../antlr4_runtime/runtime/src
)

file(GLOB sources *.cpp *.h)

add_executable(SpreadSheetFrontend ${ANTLR_FormulaParser_CXX_OUTPUTS} ${sources})

target_link_libraries(SpreadSheetFrontend PRIVATE ../libantlr4-runtime.a libkernel32.a libuser32.a libgdi32.a libwinspool.a
                                                  libcomdlg32.a libadvapi32.a libshell32.a libole32.a liboleaut32.a libuuid.a
                                                  libcomctl32.a libwsock32.a libodbc32.a)

target_link_options(SpreadSheetFrontend PRIVATE "-mthreads -mwindows")
