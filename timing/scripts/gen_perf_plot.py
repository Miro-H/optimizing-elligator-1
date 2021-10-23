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
# Generate performance plots
#
################################################################################



import argparse
import matplotlib
import matplotlib.pyplot as plt
import os

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
PLOTS_DIR_DEFAULT_PATH  = "../plots"
TITLE_FONT_SIZE = 20
LABEL_FONT_SIZE = 12

if __name__ == "__main__":
    # Read arguments
    parser = argparse.ArgumentParser()

    parser.add_argument("in_files", nargs="+",
                        help=("One or more input files to plot. Input files must "
                             "have the following structure: "
                             "Line 1: Data label; Line 2: 'x-label, y-label' "
                             "Line 3-*: 'x, y' values (separated by comma and space)"))
    parser.add_argument("--plot_fname", help="Path of the generated plot.")
    parser.add_argument("--title", help="Title for the generated plot.")
    parser.add_argument("--log_xaxis", help="Toggle x-axis to have a log scale.",
                        action="store_true")
    parser.add_argument("--log_yaxis", help="Toggle y-axis to have a log scale.",
                        action="store_true")

    args = parser.parse_args()

    in_files    = args.in_files
    plot_fname  = args.plot_fname
    plot_title  = args.title
    log_xaxis   = args.log_xaxis
    log_yaxis   = args.log_yaxis

    if plot_fname == None:
        plot_fname = f"{PLOTS_DIR_DEFAULT_PATH}/{plot_title.replace(' ', '_')}.png"

    x_label, y_label = "", ""
    is_first_data_set = True
    fig, ax = plt.subplots()

    if log_xaxis:
        ax.set_xscale("log")
    if log_yaxis:
        ax.set_yscale("log")

    color_idx = 0
    for in_file in in_files:
        with open(in_file, "r") as in_fp:
            lines = in_fp.readlines()

            data_label = lines[0].rstrip()
            data_x_label, data_y_label = lines[1].split(", ")

            if is_first_data_set:
                x_label, y_label = data_x_label, data_y_label
                is_first_data_set = False
            elif x_label != data_x_label or y_label != data_y_label:
                print("ERROR: trying to plot data with different x/y axis labels!")
                exit(1)

            xs, ys = [], []
            for line in lines[2:]:
                x, y = map(literal_eval, line.split(", "))
                xs.append(x)
                ys.append(y)

            ax.plot(xs, ys, marker='x', linewidth=.8, label=data_label,
                color=colors[color_idx])
            ax.text(xs[len(xs)//2], 0.05 + ys[len(ys)//2], data_label,
                color=colors[color_idx], fontsize=LABEL_FONT_SIZE)

            color_idx = (color_idx + 1) % len(colors)


    plt.grid(linestyle="-", axis="y", color="white")
    ax.set_xlabel(x_label)
    ax.set_ylabel(y_label, rotation=0, loc="top")
    ax.set_title(plot_title, loc="left", fontsize=TITLE_FONT_SIZE)
    ax.spines['left'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.spines['top'].set_visible(False)
    fig.tight_layout()

    fig.savefig(plot_fname, dpi=600)
    print(f"Plot stored in '{plot_fname}'")
