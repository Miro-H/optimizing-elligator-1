#!/usr/bin/env python3

import argparse
import matplotlib
import matplotlib.pyplot as plt
import os
import numpy as np
import re

from ast import literal_eval
from matplotlib import cycler

# Use nicer colors
colors = ["#0e4748", "#7fba67", "#b2552c", "#942727", "#1e2740",
          "#9bdb87", "#407294", "#660066", "#008080", "#daa520", "#8b0000",
          "#ff7373"]
plt.rc("axes", facecolor="#E6E6E6", axisbelow=True,
        prop_cycle=cycler('color', colors))

# Use MP approved font
plt.rcParams["font.sans-serif"] = "Calibri"
plt.rcParams["font.family"] = "sans-serif"
# plt.rcParams["font.style"] = "oblique"
plt.rcParams["font.size"] = 14

plt.rcParams["mathtext.default"] = "regular"

plt.rcParams["figure.figsize"] = (10,8)


#
# Constants
#
PLOTS_DEFAULT_PATH  = "./plot.png"
TITLE_FONT_SIZE = 20
LABEL_FONT_SIZE = 12

X_LABEL = "I(n) [flops/byte]"
Y_LABEL = "P(n) [flops/cycle]"

BW_LINE_LABEL = "$P(n) \\leq \\beta \cdot I(n)$"

ALU_RE_PATTERN = r"basic_(add|bitwise).*, (\d+)"
BITWISE_RE_PATTERN = r"basic_bitwise.*, (\d+)"
MUL_RE_PATTERN = r"basic_mul.*, (\d+)"
SHIFT_RE_PATTERN = r"basic_shift.*, (\d+)"
DIV_RE_PATTERN = r"basic_div.*, (\d+)"


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

    args = parser.parse_args()

    beta        = literal_eval(args.beta)
    pis_strs    = args.pis

    max_x_val = args.max_x_val

    plot_fname      = args.plot_fname
    plot_title      = args.title
    do_plot_border  = args.no_border
    logs_dirs       = args.logs_dirs
    logs_names      = args.logs_names

    if logs_dirs:
        logs_dirs = logs_dirs.split(";")

    if logs_names:
        logs_names = logs_names.split(";")

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

    for i, logs_dir in enumerate(logs_dirs):
        label = logs_names[i]

        for in_file in os.listdir(logs_dir):
            with open(os.path.join(logs_dir, in_file), "r") as in_fp:
                data = in_fp.read()

                alu_ops = re.compile(ALU_RE_PATTERN).findall(data)
                shift_ops = re.compile(SHIFT_RE_PATTERN).findall(data)
                mul_ops = re.compile(MUL_RE_PATTERN).findall(data)
                div_ops = re.compile(DIV_RE_PATTERN).findall(data)

                alu_ops_tot = sum(map(lambda x: int(x[1]), alu_ops))
                shift_ops_tot = sum(map(int, shift_ops))
                mul_ops_tot = sum(map(int, mul_ops))
                div_ops_tot = sum(map(int, div_ops))

                ops_tot = alu_ops_tot + shift_ops_tot + mul_ops_tot + div_ops_tot

                mapping_type = "map" if "string_to_point" in in_file else "inv map"
                print(f"Instruction mix for {label}, {mapping_type}:")

                if ops_tot == 0:
                    print(f"\t- NO OPS")
                else:
                    print(f"\t- ALU: {round(alu_ops_tot/ops_tot, 2)}%")
                    print(f"\t- SHIFT: {round(shift_ops_tot/ops_tot, 2)}%")
                    print(f"\t- MUL: {round(mul_ops_tot/ops_tot, 2)}%")
                    print(f"\t- DIV: {round(div_ops_tot/ops_tot, 2)}%")

                # Instruction mix indep
                if "string_to_point" in in_file:
                    # 1 BigInt, 1 curve
                    label += " str2pnt"
                    read_bw = 12672
                elif "point_to_string" in in_file:
                    # 1 curve point, 1 curve
                    label += " pnt2str"
                    read_bw = 13824
                else:
                    print("ERROR: invalid log file!")
                    exit(1)

                x = (ops_tot) / read_bw

                # Plot every point for every peak performance bound
                min_pi = min(pis)
                y_val = min(min_pi, beta * x)

                if min_pi < beta * x:
                    y_val = min_pi
                else:
                    y_val = beta * x

                dots_x.append(x)
                dots_y.append(y_val)
                dots_labels.append(label)

    #
    # Create roofline plot
    #
    color_idx = 0

    # Plot bandwidth bound
    xs = np.linspace(2**(-6), max_x_val, 1000)
    ys = [beta * x for x in xs]
    ax.loglog(xs, ys, linewidth=.8, base=2)
    ax.text(xs[-1] * 0.3, ys[-1] * 1.1, BW_LINE_LABEL,
        fontsize=LABEL_FONT_SIZE, color=colors[color_idx])

    # Plot peak performance bound(s)
    color_idx = (color_idx + 1) % len(colors)
    for i in range(len(pis)):
        xs = np.linspace(0, max_x_val, 1000)
        ys = [pis[i] for x in xs]
        ax.loglog(xs, ys, linewidth=.8, base=2)
        ax.text(xs[-1]**(1/3), ys[-1] * 1.05, f"{pi_labels[i]}; $P(n) \\leq {pis[i]}$",
            fontsize=LABEL_FONT_SIZE, color=colors[color_idx])

        if do_plot_border:
            # Plot border between memory and compute bounded areas
            intersec_x = pis[i]/beta
            ax.vlines(intersec_x, 0, pis[i], color="gray", linestyle="--", linewidth=.8)
            ax.text(intersec_x * 0.6, 0.3, f"$x = {round(pis[i]/beta, 2)}$",
                    fontsize=LABEL_FONT_SIZE, color="gray")

        color_idx = (color_idx + 1) % len(colors)

    # Plot dots
    markers = ["x", "1", "+", "2", "|", "3", "4"]
    for i in range(len(dots_x)):
        ax.plot(dots_x[i], dots_y[i], marker=markers[i],
            color=colors[color_idx], label=dots_labels[i], linestyle="None")
        color_idx = (color_idx + 1) % len(colors)

    plt.grid(linestyle="-", axis="y", color="white")
    ax.set_xlabel(X_LABEL)
    ax.set_ylabel(Y_LABEL, rotation=0, loc="top")
    ax.set_title(plot_title, loc="left", fontsize=TITLE_FONT_SIZE)
    ax.spines['right'].set_visible(False)
    ax.spines['top'].set_visible(False)

    if len(dots_x) > 0:
        ax.legend(loc="upper left", numpoints=1)

    fig.tight_layout()

    fig.savefig(plot_fname, dpi=600)
    print(f"Plot stored in '{plot_fname}'")
