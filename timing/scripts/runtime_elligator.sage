#!/usr/bin/env sage

# to get this running:
# sage -python3 -m pip install hwcounter


import random
from hwcounter import count, count_end
import argparse
import os
import datetime

input_data = []
output_data = []

class Curve1174:  # struct in c  # edward curve
    def __init__(self):
        self.q = 2^251 - 9
        self.Fq = GF(self.q)

        # Use Montogomery curve, which is equivalent to the following Edwards curve:
        # x^2 + y^2 = 1 - 1174 * x^2 * y^2
        self.curve = EllipticCurve(self.Fq, [0, 4/1175-2, 0, 1, 0])

        self.s = self.Fq(1806494121122717992522804053500797229648438766985538871240722010849934886421)
        self.c = 2 / (self.s^2)
        self.d = -(self.c + 1)^2 / (self.c - 1)^2
        self.r = self.c + 1 / self.c

        # Sanity checks
        # assert(self.q % 4 == 3)
        # assert(self.c * (self.c - 1) * (self.c + 1) != 0)
        # assert(self.d == -1174)
        # assert(self.r != 0)
        # assert(not self.d.is_square())

curve = Curve1174()

def chi(x, q):
    return legendre_symbol(x, q)

def elligator1_map(t):
    if t in [1, -1]:
        return 0, 1

    u = (1 - t) / (1 + t)
    v = u^5 + (curve.r^2 - 2) * u^3 + u

    chi_v = chi(v, curve.q)
    X = chi_v * u
    Y = (chi_v * v)^((curve.q + 1)/4) * chi_v * chi(u^2 + 1/curve.c^2, curve.q)

    x = (curve.c - 1) * curve.s * X * (1 + X) / Y
    y = (curve.r * X - (1 + X)^2) / (curve.r * X + (1 + X)^2)

    # Sanity checks
    # assert(x^2 + y^2 == 1 + curve.d * x^2 * y^2)
    # assert(u * v * X * Y * x * (y + 1) != 0)
    # assert(Y^2 == X^5 + (curve.r^2 - 2) * X^3 + X)

    return x, y

def elligator1_invmap(x, y):
    # assert(y + 1 != 0)

    eta = (y - 1) / (2 * (y + 1))

    # Two sanity checks
    # assert(((1 + eta * curve.r)^2 - 1).is_square())
    # if eta * curve.r == -2:
    #     assert(x == 2 * curve.s * (curve.c - 1) * chi(curve.c, curve.q) / curve.r)

    X = -(1 + eta * curve.r) + ((1 + eta * curve.r)^2 - 1)^((curve.q + 1) / 4)
    z = chi((curve.c - 1) * curve.s * X * (1 + X) * x * (X^2 + 1 / curve.c^2), curve.q)
    u = z * X
    t = (1 - u) / (1 + u)

    if t.lift() > (curve.q - 1) / 2:
        return -t
    return t


def elligator1_invmap_helper(x):
    return elligator1_invmap(x[0], x[1])

def elligator1_map_prep(reps):
    for rep_ in range(reps):
        input_data.append(curve.Fq((random.randint(1, 2**256))))


def elligator1_invmap_prep(reps):
    for rep_ in range(reps):
        input_data.append(elligator1_map(curve.Fq(random.randint(1, 2**256))))

def runtime(setup_f, f, title, path, sets, reps):
    output_file = open(path, "w")
    output_file.write(f"{title}\n")
    output_file.write("Measurement, Runtime [cycles]\n")

    for set_ in range(sets):
        setup_f(reps)
        start = count()
        for rep_ in range(reps):
            output_data.append(f(input_data[rep_]))
        elapsed = float((count_end() - start)) / float(reps)
        output_file.write(f"{set_}, {elapsed}\n")
        input_data.clear()
        output_data.clear()
    output_file.close()

    print(f"Wrote plot with title '{title}' to file '{path}'")

if __name__ == '__main__':
    script_dir = os.path.dirname(os.path.realpath(__file__))
    default_logs_dir = f"{script_dir}/../logs/runtime"

    # Read arguments
    parser = argparse.ArgumentParser()

    parser.add_argument("--logs_dir", help="Directory for log files",
                        default=default_logs_dir)
    parser.add_argument("--sets", help="Number of sets", default=10, type=int)
    parser.add_argument("--reps", help="Number of reps", default=1000, type=int)

    args = parser.parse_args()

    sets        = args.sets
    reps        = args.reps
    logs_dir    = args.logs_dir

    timestamp_dir_name = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    logs_dir = f"{logs_dir}/{timestamp_dir_name}"
    os.mkdir(logs_dir)

    runtime(elligator1_map_prep, elligator1_map, "Elligator str2pnt",
        f"{logs_dir}/runtime_sage_elligator_1_string_to_point.log", sets, reps)

    runtime(elligator1_invmap_prep, elligator1_invmap_helper,
        "Elligator pnt2str",
        f"{logs_dir}/runtime_sage_elligator_1_point_to_string.log", sets, reps)
