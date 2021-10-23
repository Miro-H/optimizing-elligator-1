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
# Generate plot for the function call statistics
#
################################################################################


import argparse
import matplotlib
import matplotlib.pyplot as plt
import os
import numpy as np
import re

from statistics import median
from ast import literal_eval
from itertools import cycle

# Use nicer colors
color_codes = ["#2d1a71", "#3257be", "#409def", "#70dbff", "#bfffff", "#3e32d5",
               "#6e6aff", "#a6adff", "#d8e0ff", "#652bbc", "#b44cef", "#ec8cff",
               "#ffcdff", "#480e55", "#941887", "#e444c3", "#ff91e2", "#190c12",
               "#550e2b", "#af102e", "#ff424f", "#ff9792", "#ffd5cf", "#491d1e",
               "#aa2c1e", "#f66d1e", "#ffae68", "#ffe1b5", "#492917", "#97530f",
               "#dd8c00", "#fbc800", "#fff699", "#0c101b", "#0e3e12", "#38741a",
               "#6cb328", "#afe356", "#e4fca2", "#0d384c", "#177578", "#00bc9f",
               "#6becbd", "#c9fccc", "#353234", "#665d5b", "#998d86", "#cdbfb3",
               "#eae6da", "#2f3143", "#505d6d", "#7b95a0", "#a6cfd0", "#dfeae4",
               "#8d4131", "#cb734d", "#efaf79", "#9c2b3b", "#e45761", "#ffffff",
               "#000000", "#e4162b", "#ffff40"]

hatches_strs = [ "///" , "\\\\\\" , "|||" , "-" , "+" , "x", "o", "O", ".", "*"]

plt.rc("axes", facecolor="#E6E6E6", axisbelow=True)

# Use MP approved font
plt.rcParams["font.sans-serif"] = "Calibri"
plt.rcParams["font.family"] = "sans-serif"
# plt.rcParams["font.style"] = "oblique"
plt.rcParams["font.size"] = 14

plt.rcParams["mathtext.default"] = "regular"

#
# Constants
#
PLOTS_DIR_DEFAULT_PATH  = "../plots"
LOGS_DIR_DEFAULT_PATH  = "../logs"
TITLE_FONT_SIZE = 20
LABEL_FONT_SIZE = 12


def plot(plot_title, plot_fname, log_xaxis, log_yaxis, bar_plot, ys_arr,
         x_labels, x_axis_label, y_axis_label, data_labels):
    plt.rcParams["figure.figsize"] = (14,8)
    fig, ax = plt.subplots()

    if log_yaxis:
        ax.set_yscale("log")

    if len(data_labels) == 1:
        bar_width = 0.5
    else:
        bar_width = 1 / (len(data_labels) + 2)

    colors_iter = cycle(color_codes)
    colors = [next(colors_iter) for i in range(len(x_labels))]

    hatches_iter = cycle(hatches_strs)
    hatches = [next(hatches_iter) for i in range(len(data_labels))]

    xs = np.arange(len(x_labels))

    x_off = -bar_width * len(data_labels) / 2

    for i, data_label in enumerate(data_labels):
        if bar_plot:
            if len(data_labels) > 1:
                ax.bar(xs + x_off, ys_arr[data_label], bar_width, label=data_label,
                       align="edge", color=colors, hatch=hatches[i])
            else:
                ax.bar(xs + x_off, ys_arr[data_label], bar_width, label=data_label,
                       align="edge", color=colors)
            x_off += bar_width
        else:
            ax.scatter(xs, ys_arr[data_label], marker='x', c=colors)

    plt.xticks(ticks=xs, labels=x_labels, rotation='vertical')
    plt.grid(linestyle="-", axis="y", color="white")
    ax.set_xlabel(x_axis_label)
    ax.set_ylabel(y_axis_label, rotation=0, loc="top")

    if len(data_labels) > 1:
        ax.legend()

        legend = ax.get_legend()
        for i in range(len(data_labels)):
            handle = legend.legendHandles[i]
            handle.set_color(ax.get_facecolor())
            handle.set_edgecolor("black")
            handle.set_linewidth(1)

    ax.set_title(plot_title, loc="left", fontsize=TITLE_FONT_SIZE, pad=20)
    ax.spines['left'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.spines['top'].set_visible(False)
    fig.tight_layout()

    fig.savefig(plot_fname, dpi=600)
    print(f"Plot stored in '{plot_fname}'")



if __name__ == "__main__":
    # Read arguments
    parser = argparse.ArgumentParser()

    parser.add_argument("--title_prefix", help="Title prefix for the generated plot.",
                        default="Function Call Count")
    parser.add_argument("--plot_dir", help="Path of the generated plot.")
    parser.add_argument("--logs_dirs", default=LOGS_DIR_DEFAULT_PATH,
                        help="Path to one or more directory, where the input logs are stored.")
    parser.add_argument("--logs_names",
                        help="Specify the names (separated by semicolon) of the "\
                        "data sets stored in the different log folders.")
    parser.add_argument("--log_xaxis", help="Toggle x-axis to have a log scale.",
                        action="store_true")
    parser.add_argument("--log_yaxis", help="Toggle y-axis to have a log scale.",
                        action="store_true")
    parser.add_argument("--bar_plot", help="Toggle bar plots.",
                        action="store_true")
    parser.add_argument("--pattern", help="Only plot stats matching the given regex pattern.",
                        default=r".*")

    args = parser.parse_args()

    title_prefix    = args.title_prefix
    plot_dir        = args.plot_dir
    logs_dirs       = args.logs_dirs
    logs_names      = args.logs_names
    log_xaxis       = args.log_xaxis
    log_yaxis       = args.log_yaxis
    bar_plot        = args.bar_plot
    pattern         = args.pattern

    if logs_dirs:
        logs_dirs = logs_dirs.split(";")
    if logs_names:
        logs_names = logs_names.split(";")
    else:
        logs_names = ["tmp"]

    # all log folders are supposed to have the same log file names
    for in_file in os.listdir(logs_dirs[0]):
        ys_dict = dict()
        x_labels = set()

        for i, logs_dir in enumerate(logs_dirs):
            with open(os.path.join(logs_dir, in_file), "r") as in_fp:
                lines = in_fp.readlines()

                if len(lines) == 0:
                    continue

                data_label = lines[0].rstrip()
                x_axis_label, y_axis_label = lines[1].split(", ")

                ys_dict[logs_names[i]] = dict()
                for line in lines[2:]:
                    big_int_fn, big_int_fn_cnt = line.split(", ")
                    if re.match(pattern, big_int_fn):
                        big_int_fn_cnt = literal_eval(big_int_fn_cnt.rstrip())
                        x_labels.add(big_int_fn)
                        ys_dict[logs_names[i]][big_int_fn] = big_int_fn_cnt

        # Normalize data: set entries that are not present in one folder to zero
        ys_arr = dict()
        x_labels = sorted(list(x_labels))
        for log_name in logs_names:
            ys_arr[log_name] = []
            for x_label in x_labels:
                if x_label not in ys_dict[log_name]:
                    ys_arr[log_name].append(0)
                else:
                    ys_arr[log_name].append(ys_dict[log_name][x_label])

        label_grps = {
            "add/sub": ["big_int_add", "big_int_sub"],
            "chi": ["big_int_chi", "big_int_curve1174_chi"],
            "comparisons": ["big_int_compare", "big_int_curve1174_compare_to_q", "big_int_is_odd", "big_int_is_zero"],
            "div": ["big_int_div_rem"],
            "inv": ["big_int_inv", "big_int_curve1174_inv"],
            "memory ops": ["big_int_alloc", "big_int_calloc", "big_int_copy", "big_int_destroy", "big_int_duplicate"],
            "mod": ["big_int_mod", "big_int_curve1174_mod"],
            "mul": ["big_int_mul"],
            "mul4": ["big_int_mul_4"],
            "mul single chunk": ["big_int_mul_single_chunk"],
            "other": ["big_int_neg", "big_int_abs", "big_int_sll_small", "big_int_srl_small", "big_int_prune_leading_zeros", "big_int_get_res"],
            "pow general": ["big_int_pow"],
            "pow const. exp.": ["big_int_curve1174_pow_q_m1_d2", "big_int_curve1174_pow_q_p1_d4", "big_int_curve1174_pow_q_m2"],
            "square": ["big_int_square"],
        }
        # print table in latex syntax
        print("Latex table for stats:\n\\hline")
        print(r"\textbf{function} & \textbf{\VOne{}} & "
              r"\textbf{\VTwo{}} & \textbf{\VThree{}} \\"
              + "\n\\hline")

        for grp_name, fns in label_grps.items():
            sums = []
            for log_name in logs_names:
                s = 0
                for fn in fns:
                    s += ys_arr[log_name][x_labels.index(fn)]
                sums.append(s)

            # skip entries with all zeros
            if sum(sums) == 0:
                continue

            print(grp_name, end="")
            for s in sums:
                print(f" & {s}", end="")
            print(" \\\\\n\\hline")

        title = f"{title_prefix} for '{data_label}'"
        plot_fname = plot_dir + "/" + in_file.split("/")[-1].split(".")[0] + ".png"

        plot(title, plot_fname, log_xaxis, log_yaxis, bar_plot, ys_arr, x_labels,
             x_axis_label, y_axis_label, logs_names)
