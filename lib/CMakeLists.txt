SET(jerasure_LIB_SRCS
  jerasure.c
  liberation.c
  reed_sol.h
  gal_stub.h
  gal_stub.c
  galois.c
  galois.h
  cauchy.c
  liberation.h
  reed_sol.c
  cauchy.h
  jerasure.h
)
add_library(jerasure ${jerasure_LIB_SRCS})
