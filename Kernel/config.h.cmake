
#define WITH_CMAKE
#define XML_SCHEMA "@CMAKE_INSTALL_PREFIX@/share/@PROJECT_PACKAGE_NAME@/SiconosModelSchema-V1.2.xsd"

#ifndef SVN_REVISION
#cmakedefine SVN_REVISION ${SVN_REVISION}
#endif

#cmakedefine HAVE_BLAS
#cmakedefine HAVE_LAPACK
#cmakedefine HAVE_ATLAS
#cmakedefine HAVE_CLAPACK_H
#cmakedefine HAVE_CBLAS_H
#cmakedefine HAVE_XERBLA
