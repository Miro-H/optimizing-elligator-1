#!/usr/bin/env python3

import argparse
import matplotlib
import matplotlib.pyplot as plt
import os

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


def plot(plot_title, plot_fname, log_xaxis, log_yaxis, bar_plot, xs, ys, x_labels, x_axis_label, y_axis_label):
    plt.rcParams["figure.figsize"] = (14,8)
    fig, ax = plt.subplots()

    if log_yaxis:
        ax.set_yscale("log")

    colors_iter = cycle(color_codes)
    colors = [next(colors_iter) for i in range(len(x_labels))]
    if bar_plot:
        ax.bar(xs, ys, width=0.5, color=colors)
    else:
        ax.scatter(xs, ys, marker='x', c=colors)

    plt.xticks(ticks=xs, labels=x_labels, rotation='vertical')
    plt.grid(linestyle="-", axis="y", color="white")
    ax.set_xlabel(x_axis_label)
    ax.set_ylabel(y_axis_label, rotation=0, loc="top")

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

    parser.add_argument("--plot_dir", help="Path of the generated plot.")
    parser.add_argument("--logs_dir", help="Path where the input logs are stored.",
                        default=LOGS_DIR_DEFAULT_PATH)
    parser.add_argument("--log_xaxis", help="Toggle x-axis to have a log scale.",
                        action="store_true")
    parser.add_argument("--log_yaxis", help="Toggle y-axis to have a log scale.",
                        action="store_true")
    parser.add_argument("--bar_plot", help="Toggle bar plots.",
                        action="store_true")

    args = parser.parse_args()

    plot_dir    = args.plot_dir
    logs_dir    = args.logs_dir
    log_xaxis   = args.log_xaxis
    log_yaxis   = args.log_yaxis
    bar_plot    = args.bar_plot


    for in_file in os.listdir(logs_dir):
        with open(os.path.join(logs_dir, in_file), "r") as in_fp:
            lines = in_fp.readlines()

            if len(lines) == 0:
                continue

            data_label = lines[0].rstrip()
            x_axis_label, y_axis_label = lines[1].split(", ")

            x_labels = []
            ys = []
            for line in lines[2:]:
                big_int_fn, big_int_fn_cnt = line.split(", ")
                big_int_fn_cnt = literal_eval(big_int_fn_cnt.rstrip())
                x_labels.append(big_int_fn)
                ys.append(big_int_fn_cnt)

            xs = [i for i in range(len(x_labels))]

        title = f"Function Call Count for '{data_label}'"
        plot_fname = plot_dir + "/" + in_file.split("/")[-1].split(".")[0] + ".png"
        plot(title, plot_fname, log_xaxis, log_yaxis, bar_plot, xs, ys, x_labels, x_axis_label, y_axis_label)
