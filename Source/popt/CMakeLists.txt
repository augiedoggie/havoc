
add_definitions(-DHAVE_CONFIG_H -D_GNU_SOURCE -D_REENTRANT)

set(POPT_SRCS
	popt.c
	poptconfig.c
	popthelp.c
	poptint.c
	poptparse.c)

add_library(popt ${POPT_SRCS})
