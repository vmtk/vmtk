SET (LASPACK_SRCS
    eigenval.c
		errhandl.c
		factor.c
		itersolv.c
		matrix.c
		mlsolv.c
		operats.c
		precond.c
		qmatrix.c
		rtc.c
		vector.c
    )

INCLUDE_DIRECTORIES(${LASPACK_SOURCE_DIR})

ADD_LIBRARY (laspack STATIC ${LASPACK_SRCS})
