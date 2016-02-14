import os
import shutil
from subprocess import check_call, CalledProcessError


class CiTask():

    def __init__(self,
                 mode='Continuous',
                 build_configuration='Release',
                 distrib=None,
                 ci_config=None,
                 fast=True,
                 pkgs=None,
                 srcs=None,
                 targets=None,
                 cmake_cmd='cmake'):
        self._fast = fast
        self._distrib = distrib
        self._mode = mode
        self._build_configuration = build_configuration
        self._ci_config = ci_config
        self._pkgs = pkgs
        self._srcs = srcs
        self._targets = targets
        self._cmake_cmd = cmake_cmd

    def build_dir(self, src):
        return src.replace('.', '_') + self._distrib + '_' + self._ci_config

    def templates(self):
        # remove build-base, gnu-c++, gfortran, it is redundant
        return ','.join(self._pkgs)

    def copy(self):
        def init(mode=self._mode,
                 build_configuration=self._build_configuration,
                 distrib=self._distrib,
                 ci_config=self._ci_config, fast=self._fast, pkgs=self._pkgs,
                 srcs=self._srcs, targets=self._targets, cmake_cmd=self._cmake_cmd,
                 add_pkgs=None, remove_pkgs=None, add_srcs=None,
                 remove_srcs=None, add_targets=None, remove_targets=None):

            if add_pkgs is not None:
                pkgs = self._pkgs + add_pkgs

            if remove_pkgs is not None:
                pkgs = list(filter(lambda p: p not in remove_pkgs, pkgs))

            if add_srcs is not None:
                srcs += self._srcs + add_srcs

            if remove_srcs is not None:
                srcs = list(filter(lambda p: p not in remove_srcs, srcs))

            if add_targets is not None:
                targets += self._targets + add_targets

            if remove_targets is not None:
                targets = list(filter(lambda p: p not in remove_targets, targets))

            return CiTask(mode, build_configuration, distrib, ci_config, fast,
                          pkgs, srcs, targets)
        return init

    def run(self, targets_override=None):

        return_code = 0

        for src in self._srcs:

            if targets_override is not None:
                self._targets[src] = targets_override

            bdir = self.build_dir(src)

            if os.path.exists(bdir):
                shutil.rmtree(bdir, ignore_errors=True)

            os.makedirs(bdir)

            redundants = ['build-base', 'gfortran', 'gnu-c++']
            templ_list = [p for p in self._pkgs if p not in redundants]

            cmake_args = ['-DMODE={0}'.format(self._mode),
                          '-DCI_CONFIG={0}'.format(self._ci_config),
                          '-DWITH_DOCKER=1',
                          '-DBUILD_CONFIGURATION={0}'.format(
                              self._build_configuration),
                          '-DDOCKER_DISTRIB={0}'.format(self._distrib),
                          '-DDOCKER_TEMPLATES={0}'.format(self.templates()),
                          '-DDOCKER_TEMPLATE={0}'.format(templ_list)]

            try:
                check_call([self._cmake_cmd] + cmake_args + [os.path.join('..', '..', src)],
                           cwd=bdir)

                for target in self._targets[src]:

                    check_call(['make'] + [target], cwd=bdir)

            except CalledProcessError as error:
                return_code = 1
                print(error)

        return return_code

    def clean(self):

        for src in self._srcs:

            bdir = self.build_dir(src)

            if not self._fast:

                try:
                    check_call(['make'] + ['docker-clean'],
                               cwd=bdir)

                except CalledProcessError as error:
                    print(error)

    def display(self):
        for attr, value in self.__dict__.items():
            print('\t{:} = {:}'.format(attr, value))
