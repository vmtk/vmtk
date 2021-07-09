project (STELLAR)

set (STARBASE_SRCS
    src/Starbase.c
)


set (STELLAR_SRCS
    src/Stellar.c
)

include_directories(${STELLAR_SOURCE_DIR}/src)

#add_definitions(-DSTARLIBRARY)
add_definitions(-DNOMAIN)

add_library(starbase STATIC ${STARBASE_SRCS})
add_executable(Stellar ${STELLAR_SRCS})
target_link_libraries(Stellar starbase m)

if(NOT WIN32)
  set_target_properties(starbase PROPERTIES COMPILE_FLAGS -fPIC)
  set_target_properties(Stellar PROPERTIES COMPILE_FLAGS -fPIC)
endif()

