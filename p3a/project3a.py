
import os, subprocess, shutil, time, math, random

import toolspath

from time import time
from testing import Xv6Build, Test, BuildTest, Xv6Test
from testing.runtests import main
from scipy.stats import chisquare, kstest, pearsonr
from scipy.stats import kstest


from itertools import groupby


class mprotect_basic(Xv6Test):
   name = "mprotect_basic"
   description = "check whether mprotect work properly"
   tester = "ctests/" + name + ".c"
   make_qemu_args = "CPUS=1"
   point_value = 5


class mprotect_fork(Xv6Test):
   name = "mprotect_fork"
   description = "check whether fork inherit the write bit from parent"
   tester = "ctests/" + name + ".c"
   make_qemu_args = "CPUS=1"
   point_value = 5


class munprotect_basic(Xv6Test):
   name = "munprotect_basic"
   description = "check whether munprotect work properly"
   tester = "ctests/" + name + ".c"
   make_qemu_args = "CPUS=1"
   point_value = 5
        
        
class test_protect(Xv6Test):
   name = "test_protect"
   description = "check whether mprotect and munprotect work properly"
   tester = "ctests/" + name + ".c"
   make_qemu_args = "CPUS=1"
   point_value = 8

class protect_error(Xv6Test):
   name = "protect_error"
   description = "check the error handling of mprotect and munprotect"
   tester = "ctests/" + name + ".c"
   make_qemu_args = "CPUS=1"
   point_value = 5

class allocator_a(Xv6Test):
   name = "allocator_a"
   description = "test the alternate allocator in part 2"
   tester = "ctests/" + name + ".c"
   make_qemu_args = "CPUS=1"
   point_value = 5



main(Xv6Build, [mprotect_basic, mprotect_fork ,munprotect_basic, test_protect, protect_error, allocator_a])