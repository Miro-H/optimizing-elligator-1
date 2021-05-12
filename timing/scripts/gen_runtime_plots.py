#!/usr/bin/env python3

import argparse
import matplotlib
import matplotlib.pyplot as plt
import os
import numpy as np

from statistics import median
from ast import literal_eval
from matplotlib import cycler

# Use nicer colors
colors_codes = ["#140e1e", "#2d1a71", "#3257be", "#409def", "#70dbff",
                "#bfffff", "#3e32d5", "#6e6aff", "#a6adff", "#d8e0ff", "#652bbc",
                "#b44cef", "#ec8cff", "#ffcdff", "#480e55", "#941887", "#e444c3",
                "#ff91e2", "#190c12", "#550e2b", "#af102e", "#ff424f", "#ff9792",
                "#ffd5cf", "#491d1e", "#aa2c1e", "#f66d1e", "#ffae68", "#ffe1b5",
                "#492917", "#97530f", "#dd8c00", "#fbc800", "#fff699", "#0c101b",
                "#0e3e12", "#38741a", "#6cb328", "#afe356", "#e4fca2", "#0d384c",
                "#177578", "#00bc9f", "#6becbd", "#c9fccc", "#353234", "#665d5b",
                "#998d86", "#cdbfb3", "#eae6da", "#2f3143", "#505d6d", "#7b95a0",
                "#a6cfd0", "#dfeae4", "#8d4131", "#cb734d", "#efaf79", "#9c2b3b",
                "#e45761", "#ffffff", "#000000", "#e4162b", "#ffff40"]
color_cycler = cycler('color', colors_codes)()

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


def plot(plot_title, plot_fname, log_xaxis, log_yaxis, bar_plot, logs_dirs):
    plt.rcParams["figure.figsize"] = (14,8)

    x_label, y_label = "", ""
    is_first_data_set = True
    fig, ax = plt.subplots()

    if log_yaxis:
        ax.set_yscale("log")

    color_idx = 0

    first = True
    x_labels = []
    versions = []
    ys = dict()
    for logs_dir in logs_dirs:
        version = logs_dir.split("/")[-1].replace("_", " ")
        versions.append(version)
        ys[version] = []

        for i, in_file in enumerate(os.listdir(logs_dir)):
            print(in_file)
            with open(os.path.join(logs_dir, in_file), "r") as in_fp:
                lines = in_fp.readlines()

                if len(lines) == 0:
                    continue

                data_label = lines[0].rstrip()
                _, data_y_label = lines[1].split(", ")
                data_x_label = "Benchmark"

                if is_first_data_set:
                    x_label, y_label = data_x_label, data_y_label
                    is_first_data_set = False
                elif x_label != data_x_label or y_label != data_y_label:
                    print("ERROR: trying to plot data with different x/y axis labels!")
                    exit(1)

                cycles = []
                for line in lines[2:]:
                    cycles.append(literal_eval(line.split(", ")[1]))

                cycles_median = median(cycles)

                ys[version].append(cycles_median)
                if first:
                    x_labels.append(data_label)
                elif x_labels[i] != data_label:
                    print("ERROR: parsing error, all folders are required to " \
                          "have the same log files (in the same order) and for " \
                          "the same data labels!")
                    exit(1)
        first = False

    xs = np.arange(len(ys))
    colors = [next(color_cycler)["color"] for i in range(len(ys))]
    nr_of_versions = len(nr_of_versions)
    bar_width = 1 / nr_of_versions + 2

    fig, ax = plt.subplots()

    x_off = -bar_width * int(len(prefetchers)/2)
    for version in versions:
        if bar_plot:
            ax.bar(xs + x_off, ys[version], bar_width,
                   label=version, align="center")
            x_off += bar_width
        else:
            ax.scatter(xs, ys, marker='x', c=colors)

    plt.xticks(ticks=xs, labels=x_labels, rotation='vertical')
    plt.grid(linestyle="-", axis="y", color="white")
    ax.set_xlabel(x_label)
    ax.set_ylabel(y_label, rotation=0, loc="top")

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

    parser.add_argument("--title", help="Title for the generated plot.")
    parser.add_argument("--plot_fname", help="Path of the generated plot.")
    parser.add_argument("--logs_dirs", nargs="*",
                        help="Path where the input logs are stored. If multiple " \
                        "directories are given, this expects multiple log files " \
                        "with the same data name and will do a comparison for them.",
                        default=LOGS_DIR_DEFAULT_PATH)
    parser.add_argument("--log_xaxis", help="Toggle x-axis to have a log scale.",
                        action="store_true")
    parser.add_argument("--log_yaxis", help="Toggle y-axis to have a log scale.",
                        action="store_true")
    parser.add_argument("--bar_plot", help="Toggle bar plots.",
                        action="store_true")

    args = parser.parse_args()

    title       = args.title
    plot_fname  = args.plot_fname
    logs_dirs   = args.logs_dirs
    log_xaxis   = args.log_xaxis
    log_yaxis   = args.log_yaxis
    bar_plot    = args.bar_plot

    if len(logs_dirs) > 1 and not bar_plot:
        print("ERROR: comparison plots are only supported for bar plots.")
        exit(1)

    plot(title, plot_fname, log_xaxis, log_yaxis, bar_plot, logs_dirs)
