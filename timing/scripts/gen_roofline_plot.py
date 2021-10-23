#!/usr/bin/env python3


################################################################################
#
# This file is part of the ASL project "Censorship-avoiding high-speed EC
# (Elligator with Curve1174)"
# (https://github.com/Miro-H/optimizing-elligator-1).
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
# Contact: miro.haller@alumni.ethz.ch
#
# Short description of this file:
# Generate roofline plot
#
################################################################################


import argparse
import matplotlib
import matplotlib.pyplot as plt
import os
import numpy as np
import re

from ast import literal_eval
from matplotlib import cycler

# Use nicer colors
colors = ["#0e4748", "#407294", "#b2552c", "#942727", "#1e2740",
          "#7fba67", "#660066", "#008080", "#daa520", "#8b0000", "#ff7373"]
plt.rc("axes", facecolor="#E6E6E6", axisbelow=True,
        prop_cycle=cycler('color', colors))

# Use MP approved font
plt.rcParams["font.sans-serif"] = "Calibri"
plt.rcParams["font.family"] = "sans-serif"
# plt.rcParams["font.style"] = "oblique"
plt.rcParams["font.size"] = 14

plt.rcParams["mathtext.default"] = "regular"

plt.rcParams["figure.figsize"] = (10,8)

plt.rcParams["text.usetex"] = True

#
# Constants
#
PLOTS_DEFAULT_PATH  = "./plot.png"
TITLE_FONT_SIZE = 28
LABEL_FONT_SIZE = 20

X_LABEL = "I(n) [iops/byte]"
Y_LABEL = "P(n) [iops/cycle]"

BW_LINE_LABEL = "$P(n) \\leq \\beta \cdot I(n)$"

AVX_ALU_RE_PATTERN = r"avx_(add|bitwise|other).*, (\d+)"
ALU_RE_PATTERN = r"basic_(add|bitwise|other).*, (\d+)"
BITWISE_RE_PATTERN = r"basic_bitwise.*, (\d+)"
AVX_MUL_RE_PATTERN = r"avx_mul.*, (\d+)"
MUL_RE_PATTERN = r"basic_mul.*, (\d+)"
SHIFT_RE_PATTERN = r"basic_shift.*, (\d+)"
DIV_RE_PATTERN = r"basic_div.*, (\d+)"

# We assume all our BigInts are 64 bits, so only 4 fit into one 256 bit vector
OPS_PER_VEC = 4

PRINT_INSTR_MIX = False

if __name__ == "__main__":
    # Read arguments
    parser = argparse.ArgumentParser()

    parser.add_argument("--title", help="Title for the generated plot.")

    # Architecture info
    parser.add_argument("--beta", required=True, help="Memory bandwidth.")
    parser.add_argument("--pis", required=True, nargs="+",
                        help="Peak performance value and label as string: 'val;label'.")

    parser.add_argument("--max_x_val", required=True, type=int,
                        help="Maximum intensity for the x-axis.")

    parser.add_argument("--no_border", action="store_false", default=True,
                        help=("Do not plot the border between memory "
                              "and compute bounded areas"))
    parser.add_argument("--plot_fname", help="Path of the generated plot.",
                        default=PLOTS_DEFAULT_PATH)
    parser.add_argument("--logs_names", help="Labels for each log file.")
    parser.add_argument("--logs_dirs", help="Log files with the stat counts.")
    parser.add_argument("--pis_idx", help="Index of upper bound relevant for this data set.")
    parser.add_argument("--runtimes", help="Runtime for this data set.")

    args = parser.parse_args()

    beta        = literal_eval(args.beta)
    pis_strs    = args.pis

    max_x_val = args.max_x_val

    plot_fname      = args.plot_fname
    plot_title      = args.title
    do_plot_border  = args.no_border
    logs_dirs       = args.logs_dirs
    logs_names      = args.logs_names
    pis_idx         = args.pis_idx
    runtimes        = args.runtimes

    if logs_dirs:
        logs_dirs = logs_dirs.split(";")

    if logs_names:
        logs_names = logs_names.split(";")

    if pis_idx:
        pis_idx = list(map(int, pis_idx.split(";")))

    if runtimes:
        runtimes = list(map(int, runtimes.split(";")))

    fig, ax = plt.subplots()

    #
    # Parse general architecture input
    #

    # Get peak performance values
    pis = []
    pi_labels = []
    for pis_str in pis_strs:
        pi, pi_label = pis_str.split(";")
        pis.append(literal_eval(pi))
        pi_labels.append(pi_label.rstrip())

    dots_x = []
    dots_y = []
    dots_labels = []

    VERSION_MACROS = { "V1": "\VOne{}", "V2": "\VTwo{}", "V3": "\VThree{}" }
    FN_MACROS = {"str2pnt": "\StrToPnt{}", "pnt2str": "\PntToStr{}" }
    print("Instruction mix table:\n\hline\\textbf{version} & \\textbf{function} & \\textbf{\\execALU{}} "
          + "& \\textbf{\\execShift{}} & \\textbf{\\execMul{}} & \\textbf{\\execDiv{}} \\\\")
    idx = 0
    for i, logs_dir in enumerate(logs_dirs):
        # XXX: Makes the assumption that we plot 2 values per version
        print(f"\\hline\n\\multirow{{2}}{{*}}{{{VERSION_MACROS[logs_names[i]]}}}", end="")

        for in_file in os.listdir(logs_dir):
            with open(os.path.join(logs_dir, in_file), "r") as in_fp:
                data = in_fp.read()

                avx_alu_ops = re.compile(AVX_ALU_RE_PATTERN).findall(data)
                alu_ops = re.compile(ALU_RE_PATTERN).findall(data)
                shift_ops = re.compile(SHIFT_RE_PATTERN).findall(data)
                mul_ops = re.compile(MUL_RE_PATTERN).findall(data)
                avx_mul_ops = re.compile(AVX_MUL_RE_PATTERN).findall(data)
                div_ops = re.compile(DIV_RE_PATTERN).findall(data)

                alu_ops_tot = sum(map(lambda x: int(x[1]), alu_ops))
                avx_alu_ops_tot = sum(map(lambda x: int(x[1]), avx_alu_ops))
                shift_ops_tot = sum(map(int, shift_ops))
                mul_ops_tot = sum(map(int, mul_ops))
                avx_mul_ops_tot = sum(map(int, avx_mul_ops))
                div_ops_tot = sum(map(int, div_ops))

                instr_tot = alu_ops_tot + avx_alu_ops_tot + shift_ops_tot \
                            + mul_ops_tot + avx_mul_ops_tot + div_ops_tot

                ops_tot = alu_ops_tot + OPS_PER_VEC * avx_alu_ops_tot + shift_ops_tot \
                            + mul_ops_tot + OPS_PER_VEC * avx_mul_ops_tot + div_ops_tot

                mapping_type = "str2pnt" if "string_to_point" in in_file else "pnt2str"
                # print(f"Instruction mix for {label}, {mapping_type}:")

                if instr_tot == 0:
                    print(f"\t- NO INSTRUCTIONS")
                else:
                    if PRINT_INSTR_MIX:
                        print(f" & {FN_MACROS[mapping_type]} & "
                              + f"{round(100 * (alu_ops_tot + avx_alu_ops_tot)/instr_tot, 1)}\ & "
                              + f"{round(100 * shift_ops_tot/instr_tot, 1)} & "
                              + f"{round(100 * (mul_ops_tot + avx_mul_ops_tot)/instr_tot, 1)} & "
                              + f"{round(100 * div_ops_tot/instr_tot, 1)} \\\\")
                    else:
                        print(f" & {FN_MACROS[mapping_type]} & "
                              + f"{instr_tot} \\\\")

                # Instruction mix indep
                if "string_to_point" in in_file:
                    # 1 BigInt, 1 curve
                    read_bw = 12672
                elif "point_to_string" in in_file:
                    # 1 curve point, 1 curve
                    read_bw = 13824
                else:
                    print("ERROR: invalid log file!")
                    exit(1)

                x = ops_tot / read_bw

                # Upper bound
                y_max = min(pis[pis_idx[idx]], beta * x)
                # y = #iops / #runtime
                y = min(y_max, ops_tot / runtimes[idx])

                label = f"{logs_names[i]} {mapping_type}"
                dots_x.append(x)
                dots_y.append(y)
                dots_labels.append(label)

                idx += 1
    print("\\hline")
    #
    # Create roofline plot
    #
    color_idx = 0

    # Plot bandwidth bound
    xs = np.linspace(2**(-3), max_x_val, 1000)
    ys = [beta * x for x in xs]
    ax.loglog(xs, ys, linewidth=.8, base=2)
    ax.text(xs[-1] * 0.05, ys[-1] * 1.05, BW_LINE_LABEL,
        fontsize=LABEL_FONT_SIZE, color=colors[color_idx])

    # Plot peak performance bound(s)
    color_idx = (color_idx + 1) % len(colors)
    for i in range(len(pis)):
        xs = np.linspace(0, max_x_val, 1000)
        ys = [pis[i] for x in xs]
        ax.loglog(xs, ys, linewidth=.8, base=2)
        ax.text(xs[-1]**(1/3), ys[-1] * 1.2, f"{pi_labels[i]}; $P(n) \\leq {pis[i]}$",
            fontsize=LABEL_FONT_SIZE, color=colors[color_idx])

        if do_plot_border:
            # Plot border between memory and compute bounded areas
            intersec_x = pis[i]/beta
            ax.vlines(intersec_x, 0, pis[i], color="gray", linestyle="--", linewidth=.8)
            # ax.text(intersec_x * 0.6, 0.3, f"$x = {round(pis[i]/beta, 2)}$",
            #         fontsize=LABEL_FONT_SIZE, color="gray")

        color_idx = (color_idx + 1) % len(colors)

    # Plot dots
    markers = ["o", "^", "x", "1", "+", "2", "|", "3", "4"]

    # XXX: Makes the assumption that we plot x versions and for each version two functions!
    TYPES_PER_VERSION = 2
    for i in range(len(dots_x)):
        ax.plot(dots_x[i], dots_y[i], marker=markers[i % TYPES_PER_VERSION],
            color=colors[color_idx], label=dots_labels[i], linestyle="None")

        if i % 2 == 1:
            color_idx = (color_idx + 1) % len(colors)

    plt.grid(linestyle="-", axis="y", color="white")
    ax.set_xlabel(X_LABEL, fontsize=LABEL_FONT_SIZE)
    # ax.set_ylabel(Y_LABEL, rotation=0, loc="top")
    plt.title(f"\\fontsize{{{TITLE_FONT_SIZE}pt}}{{3em}}\\selectfont{{}}{{{plot_title}}}\n"
              + f"\\fontsize{{{LABEL_FONT_SIZE}pt}}{{3em}}\\selectfont{{}}{{{Y_LABEL}}}",
              loc="left")

    ax.spines['right'].set_visible(False)
    ax.spines['top'].set_visible(False)

    if len(dots_x) > 0:
        ax.legend(loc="upper left", numpoints=1, fontsize=LABEL_FONT_SIZE)

    fig.tight_layout()

    fig.savefig(plot_fname, dpi=600)
    print(f"Plot stored in '{plot_fname}'")
