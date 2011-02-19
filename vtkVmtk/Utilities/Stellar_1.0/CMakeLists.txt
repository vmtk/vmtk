CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT (STELLAR)

SET (STARBASE_SRCS
    src/Starbase.c
)


SET (STELLAR_SRCS
    src/Stellar.c
)


#SET (STELLAR_SRCS
#    src/Stellar.c
#    src/arraypoolstack.c
#    src/classify.c
#    src/insertion.c 
#    src/interact.c
#    src/journal.c
#    src/main.c
#    src/output.c
#    src/print.c
#    src/quadric.c 
#    src/anisotropy.c 
#    src/quality.c 
#    src/size.c 
#    src/smoothing.c 
#    src/top.c 
#    src/topological.c 
#    src/vector.c 
#    src/improve.c 
#    src/Starbase.c
#)

INCLUDE_DIRECTORIES(${STELLAR_SOURCE_DIR}/src)

#ADD_DEFINITIONS(-DSTARLIBRARY)
ADD_DEFINITIONS(-DNOMAIN)

ADD_LIBRARY(starbase STATIC ${STARBASE_SRCS})
ADD_EXECUTABLE(Stellar ${STELLAR_SRCS})
TARGET_LINK_LIBRARIES(Stellar starbase)

IF(NOT WIN32)
  SET_TARGET_PROPERTIES(starbase PROPERTIES COMPILE_FLAGS -fPIC)
  SET_TARGET_PROPERTIES(Stellar PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF(NOT WIN32)

