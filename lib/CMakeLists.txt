SET(jerasure_LIB_SRCS
  jerasure.c
  liberation.c
  reed_sol.h
  gal_stub.h
  gal_stub.c
  cauchy.c
  liberation.h
  reed_sol.c
  cauchy.h
  jerasure.h
)
if(NOT WITH_GF)
  SET(jerasure_LIB_SRCS ${jerasure_LIB_SRCS} galois.c galois.h gal_stub.c gal_stub.h)
endif(NOT WITH_GF)
add_library(jerasure ${jerasure_LIB_SRCS})
if(WITH_GF)
  target_link_libraries (jerasure ${gf_library})
endif(WITH_GF)
