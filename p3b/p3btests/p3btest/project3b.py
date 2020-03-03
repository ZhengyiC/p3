
import os, subprocess, shutil, time, math, random

import toolspath

from time import time
from testing import Xv6Build, Test, BuildTest, Xv6Test
from testing.runtests import main
from scipy.stats import chisquare, kstest, pearsonr
from scipy.stats import kstest


from itertools import groupby

class RandTest(Test):
    targets = ['rander_tester']
    point_value = 5
    name = 'statistical test'
    status = 0
    description = 'compile and test the uniformity and randomness of the random number generator'
    timeout = 20

    def run_rand_tester(self, rand_range, seed_num):

        infd = None
        outfd = subprocess.PIPE
        errfd = subprocess.PIPE
        start = time()
        command = ['./rand_tester', str(seed_num), str(rand_range)]
        child = self.startexe(command, None, None,
            stdin=infd, stdout=outfd, stderr=errfd)
        (outdata, errdata) = child.communicate()

        child.wallclock_time = time() - start

        return outdata.split()

    def run(self):
        build_command = ['gcc', '-Wl,-wrap,rand', self.test_path + '/ctests/rand_tester.c', 'kernel/rand.c', '-I./include', '-I./user' , '-I.kernel/','-o', 'rand_tester', '-Wall', '-Werror']

        rt_code = self.run_util(build_command)

        random.seed(time())
        num_pass = 0
        for i in range(5):
            rand_range = 2147483647
            seed_num = random.randint(0, 5967)

            out = self.run_rand_tester(rand_range, seed_num)
            if len(out) != 100000:
                self.raise_failure("rand_tester exits accidently")
            
            out = [float(num) / rand_range for num in out]
            
            _, p_val_ks = kstest(out, 'uniform')

            if not math.isnan(p_val_ks) and p_val_ks >= 0.05:
                num_pass = num_pass + 1
        
        if num_pass < 3:
            self.raise_failure(str(self.name) + " Test: PRNG failed on Kolmogorov-Smirnov test.")
        # Test correlation
        num_pass = 0
        for i in range(5):
            seed_num_1 = 0
            seed_num_2 = 0
            while seed_num_1 == seed_num_2:
                seed_num_1 = random.randint(0, 5967)
                seed_num_2 = random.randint(0, 5967)
        
            out_1 = self.run_rand_tester(rand_range, seed_num_1)
            out_2 = self.run_rand_tester(rand_range, seed_num_2)
            out_1 = [int(num) for num in out_1]
            out_2 = [int(num) for num in out_2]

            _, p_val_pearson = pearsonr(out_1, out_2)
            num_pass = num_pass + 1
            if math.isnan(p_val_pearson) or p_val_pearson < 0.05:
                num_pass = num_pass - 1
        if num_pass < 3:
            self.raise_failure(str(self.name) + " Test: PRNG failed on pearson correlation test.")
        self.done()
    
        
class SrandTest(Test):
    targets = ['test_srand']
    point_value = 3
    name = 'srand behavior test'
    status = 0
    description = 'test the behavior of srand according to the definition in the header file'
    timeout = 20

    def run(self):
        build_command = ['gcc', '-Wl,-wrap,rand', self.test_path + '/ctests/srand_tester.c', 'kernel/rand.c', '-I./include', '-I./user', '-I./kernel', '-o', 'srand_tester', '-Wall', '-Werror']
        rt_code = self.run_util(build_command)

        infd = None
        outfd = subprocess.PIPE
        errfd = subprocess.PIPE
        start = time()
        command = ['./srand_tester']
        child = self.run_util(command)
        self.done()


class Allocator_b(Xv6Test):
   name = "allocator_b"
   description = "test the random allocator in part 3"
   tester = "ctests/" + name + ".c"
   make_qemu_args = "CPUS=1"
   rand_test = True
   point_value = 5


main(Xv6Build, [RandTest, SrandTest, Allocator_b])
