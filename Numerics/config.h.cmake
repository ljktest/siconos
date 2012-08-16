

#define WITH_CMAKE

#ifndef SVN_REVISION
#cmakedefine SVN_REVISION ${SVN_REVISION}
#endif

#cmakedefine USE_MKL
#cmakedefine HAVE_BLAS
#cmakedefine HAVE_LAPACK
#cmakedefine HAVE_ATLAS
#cmakedefine HAVE_CLAPACK_H
#cmakedefine HAVE_CBLAS_H
#cmakedefine HAVE_ATLAS_AUX_H
#cmakedefine FRAMEWORK_BLAS
#cmakedefine HAVE_XERBLA
#cmakedefine COMPLETE_LAPACK_LIBRARIES
#cmakedefine HAVE_PATHFERRIS
#cmakedefine HAVE_MLCPSIMPLEX
#cmakedefine HAVE_TIME_H
#cmakedefine HAVE_SYSTIMES_H
#cmakedefine WITH_MUMPS
#cmakedefine WITH_TIMERS
#cmakedefine DUMP_PROBLEM
#cmakedefine WITH_FCLIB
#cmakedefine BUILD_AS_CPP
