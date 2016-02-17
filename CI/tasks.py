from machinery.ci_task import CiTask

siconos_default = CiTask(
    ci_config='default',
    distrib='ubuntu:14.04',
    pkgs=['build-base', 'gcc', 'gfortran', 'gnu-c++', 'atlas-lapack', 'lpsolve'],
    srcs=['.'],
    targets={'.': ['docker-build', 'docker-ctest']})

siconos_debian_latest = siconos_default.copy()(
    ci_config='with_bullet',
    distrib='debian:latest')

siconos_ubuntu_14_10 = siconos_default.copy()(
    distrib='ubuntu:14.10')

siconos_ubuntu_15_04 = siconos_default.copy()(
    distrib='ubuntu:15.04')

siconos_ubuntu_15_10 = siconos_default.copy()(
    distrib='ubuntu:15.10')

siconos_profiling = siconos_ubuntu_15_10.copy()(
    build_configuration='Profiling')

# note fedora/atlas-lapack in siconos.yml -> cmake does not detect blas
siconos_fedora_latest = siconos_default.copy()(
    distrib='fedora:latest',
    remove_pkgs=['atlas-lapack'],
    add_pkgs=['openblas-lapacke'])

siconos_openblas_lapacke = siconos_default.copy()(
    remove_pkgs=['atlas-lapack'],
    add_pkgs=['openblas-lapacke'])

siconos_clang = siconos_ubuntu_15_10.copy()(
    ci_config='with_bullet',
    add_pkgs=['clang'])

siconos_clang_asan = siconos_clang.copy()(
    ci_config='with_asan_clang',
    add_pkgs=['mumps', 'hdf5'])

# <clang-3.7.1 does not support linux 4.2
# This will likely hurt you
siconos_clang_msan = siconos_clang.copy()(
    ci_config='with_msan',
    add_pkgs=['libcxx_msan', 'wget', 'xz'])

siconos_gcc_asan = siconos_fedora_latest.copy()(
    ci_config='with_asan',
    cmake_cmd='Build/ci-scripts/fedora-mpi.sh',
    add_pkgs=['mumps', 'hdf5', 'asan'])

siconos_gcc_asan_latest = siconos_fedora_latest.copy()(
    ci_config='with_asan',
    distrib='fedora:rawhide',
    cmake_cmd='Build/ci-scripts/fedora-mpi.sh',
    add_pkgs=['mumps', 'hdf5', 'asan'])

siconos_serialization = siconos_default.copy()(
    ci_config='with_serialization',
    add_pkgs=['serialization'])

siconos_with_mumps = siconos_default.copy()(
    ci_config='with_mumps',
    add_pkgs=['mumps'])


siconos_default_examples = siconos_default.copy()(
    ci_config='examples',
    srcs=['.', 'examples'],
    targets={'.': ['docker-build', 'docker-cmake', 'docker-make',
                   'docker-make-install'],
             'examples': ['docker-build', 'docker-ctest']})


# dispatch based on hostname and distrib type (to min. disk requirement)

known_tasks = {'siconos---vm0':
               (siconos_fedora_latest,
                siconos_gcc_asan,
                siconos_gcc_asan_latest),

               'siconos---vm1':
               (siconos_default_examples,
                siconos_clang,
                siconos_clang_asan,
                siconos_clang_msan),

               'siconos---vm2':
               (siconos_ubuntu_15_10,
                siconos_ubuntu_15_04,
                siconos_ubuntu_14_10,
                siconos_default_profiling),

               'siconos---vm3':
               (siconos_debian_latest,
                siconos_openblas_lapacke,
                siconos_serialization,
                siconos_with_mumps)}
