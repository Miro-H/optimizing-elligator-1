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
# Generate runtime plot
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
TITLE_FONT_SIZE = 28
LABEL_FONT_SIZE = 20


def plot(plot_title, plot_fname, log_xaxis, log_yaxis, bar_plot, speedup_plot, logs_dirs, logs_names, pattern):
    plt.rcParams["figure.figsize"] = (8,8)

    x_label, y_label = "", ""
    is_first_data_set = True
    fig, ax = plt.subplots()

    if not logs_names:
        logs_names = ["tmp"]

    color_idx = 0

    first = True
    y_labels = []
    versions = []
    xs = dict()
    for i, logs_dir in enumerate(logs_dirs):
        version = logs_names[i]
        versions.append(version)
        xs[version] = dict()

        for i, in_file in enumerate(os.listdir(logs_dir)):
            with open(os.path.join(logs_dir, in_file), "r") as in_fp:
                lines = in_fp.readlines()

                if len(lines) == 0:
                    continue

                data_label = lines[0].rstrip()
                _, data_x_label = lines[1].split(", ")
                data_y_label = "Benchmark"

                if is_first_data_set:
                    y_labelTODO, x_label = data_y_label, data_x_label
                    is_first_data_set = False
                elif y_labelTODO != data_y_label or x_label != data_x_label:
                    print("ERROR: trying to plot data with different x/y axis labels!")
                    exit(1)

                cycles = []
                for line in lines[2:]:
                    cycles.append(literal_eval(line.split(", ")[1]))

                cycles_median = median(cycles)
                xs[version][data_label] = cycles_median

        first = False

    # Gather all data labels
    y_labels = set()
    for version in versions:
        for y_label in xs[version].keys():
            if y_label not in y_labels and re.match(pattern, y_label):
                y_labels.add(y_label)
    y_labels = sorted(list(y_labels))


    # Normalize data and add zero values for non-existant data
    xs_norm = dict()
    for version in versions:
        xs_norm[version] = []
        for y_label in y_labels:
            if y_label not in xs[version]:
                xs_norm[version].append(0)
            else:
                if speedup_plot:
                    # normalize all data in relation to first version
                    xs_norm[version].append(xs[versions[0]][y_label] / xs[version][y_label])
                else:
                    xs_norm[version].append(xs[version][y_label])

    # print latex table of speedup plot
    if speedup_plot:
        print("Latex table for speedup compared to V1:\n\\hline")
        print(r"\textbf{function} & \textbf{\VTwo{}} & \textbf{\VThree{}} \\"
              + "\n\\hline")

        for i in range(len(y_labels)):
            print(y_labels[i], end="")
            for version in versions[1:]:
                speedup = round(xs_norm[version][i], 1)
                print(f" & {speedup}", end="")
            print("\\\\\n\\hline")
    else:
        print("Latex table for runtime:\n\\hline")
        VERSION_MACROS = {"V1": r"\VOne{}", "V2": r"\VTwo{}", "V3": r"\VThree{}"}

        print(r"\textbf{function}", end="", )
        for version in versions:
            if version not in VERSION_MACROS:
                # WARING, using V3 as default version!
                version_label = VERSION_MACROS["V3"]
            else:
                version_label = VERSION_MACROS[version]

            print(f" & \\textbf{{{version_label}}}", end="")
        print(" \\\\\n\\hline")

        for i in range(len(y_labels)):
            print(y_labels[i], end="")
            for version in versions:
                cycles = round(xs_norm[version][i])
                print(f" & {cycles}", end="")
            print(" \\\\\n\\hline")

    ys = np.arange(len(y_labels))
    nr_of_versions = len(versions)

    colors_iter = cycle(color_codes)
    colors = [next(colors_iter) for i in range(len(y_labels))]

    hatches_iter = cycle(hatches_strs)
    hatches = [next(hatches_iter) for i in range(nr_of_versions)]

    if nr_of_versions == 1:
        bar_width = 0.5
    else:
        bar_width = 1 / (nr_of_versions + 2)

    fig, ax = plt.subplots()

    y_off = -bar_width * len(versions) / 2
    for i, version in enumerate(versions):
        if bar_plot or speedup_plot:
            if nr_of_versions > 1 or speedup_plot:
                ax.barh(ys + y_off, xs_norm[version], bar_width, label=version,
                       align="edge", color=colors, hatch=hatches[i])
            else:
                ax.barh(ys + y_off, xs_norm[version], bar_width, label=version,
                       align="edge", color=colors)
            y_off += bar_width
        else:
            ax.scatter(ys, xs_norm[version], marker='x', c=colors)

    ax.set_yticks(ys)
    ax.set_yticklabels(y_labels, fontsize=LABEL_FONT_SIZE)
    ax.invert_yaxis()  # labels read top-to-bottom
    plt.grid(linestyle="-", axis="y", color="white")

    if speedup_plot:
        x_label = "speedup"
    ax.set_xlabel(x_label, fontsize=LABEL_FONT_SIZE)

    if nr_of_versions > 1:
        ax.legend()

        legend = ax.get_legend()
        for i in range(nr_of_versions):
            handle = legend.legendHandles[i]
            handle.set_color(ax.get_facecolor())
            handle.set_edgecolor("black")
            handle.set_linewidth(1)

    ax.set_title(plot_title, loc="left", fontsize=TITLE_FONT_SIZE, pad=20)
    ax.spines['left'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.spines['top'].set_visible(False)

    if log_yaxis:
        ax.set_yscale("log")
    if log_xaxis:
        ax.set_xscale("log")

    fig.tight_layout()

    fig.savefig(plot_fname, dpi=600)
    print(f"Plot stored in '{plot_fname}'")

if __name__ == "__main__":
    # Read arguments
    parser = argparse.ArgumentParser()

    parser.add_argument("--title", help="Title for the generated plot.")
    parser.add_argument("--plot_fname", help="Path of the generated plot.")
    parser.add_argument("--logs_dirs",
                        help="Path where the input logs are stored. If multiple " \
                        "directories are given, this expects multiple log files " \
                        "(separated by semicolon) with the same data name and " \
                        "will do a comparison for them.",
                        default=LOGS_DIR_DEFAULT_PATH)
    parser.add_argument("--logs_names",
                        help="Specify the names (separated by semicolon) of the "\
                        "data sets stored in the different log folders.")
    parser.add_argument("--log_xaxis", help="Toggle x-axis to have a log scale.",
                        action="store_true")
    parser.add_argument("--log_yaxis", help="Toggle y-axis to have a log scale.",
                        action="store_true")
    parser.add_argument("--bar_plot", help="Toggle bar plots.",
                        action="store_true")
    parser.add_argument("--speedup_plot", help="Toggle speedup plots. The first "\
                        "log name is taken as baseline.",
                        action="store_true")
    parser.add_argument("--pattern", help="Only plot stats matching the given regex pattern.",
                        default=r".*")

    args = parser.parse_args()

    title           = args.title
    plot_fname      = args.plot_fname
    logs_dirs       = args.logs_dirs
    logs_names      = args.logs_names
    log_xaxis       = args.log_xaxis
    log_yaxis       = args.log_yaxis
    bar_plot        = args.bar_plot
    speedup_plot    = args.speedup_plot
    pattern         = args.pattern

    if logs_dirs:
        logs_dirs = logs_dirs.split(";")
    if logs_names:
        logs_names = logs_names.split(";")

    if logs_names and len(logs_names) != len(logs_dirs):
        print("ERROR: need to name all log folders!")
        exit(1)

    if len(logs_dirs) > 1 and not bar_plot and not speedup_plot:
        print("ERROR: comparisons are only supported for bar or speedup plots.")
        exit(1)

    plot(title, plot_fname, log_xaxis, log_yaxis, bar_plot, speedup_plot, logs_dirs, logs_names, pattern)
