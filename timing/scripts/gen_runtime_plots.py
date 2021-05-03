#!/usr/bin/env python3

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
LOGS_DIR_DEFAULT_PATH  = "../logs"
TITLE_FONT_SIZE = 20
LABEL_FONT_SIZE = 12


def plot(plot_title, plot_fname, log_yaxis, bar):

    x_label, y_label = "", ""
    is_first_data_set = True
    fig, ax = plt.subplots()

    if log_yaxis:
        ax.set_yscale("log")

    color_idx = 0

    xs, ys = [], []
    for i, in_file in enumerate(os.listdir(LOGS_DIR_DEFAULT_PATH)):
        with open(os.path.join(LOGS_DIR_DEFAULT_PATH, in_file), "r") as in_fp:
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

            y_sum = 0
            c = 0
            for line in lines[2:]:
                x, y = map(literal_eval, line.split(", "))
                y_sum += y
                c += 1

            xs.append(in_file.split(".")[0])
            ys.append(y_sum/c)

            if bar:
                ax.bar(in_file.split(".")[0], y_sum/c, linewidth=.8, color=colors[color_idx])
            else:
                ax.plot(in_file.split(".")[0], y_sum/c, marker='x', linewidth=.8, color=colors[color_idx])

            ax.tick_params(axis='x', rotation=90)
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



if __name__ == "__main__":
    # Read arguments
    parser = argparse.ArgumentParser()

    plot_fname_1 = f"{PLOTS_DIR_DEFAULT_PATH}/overview.png"
    plot_fname_2 = f"{PLOTS_DIR_DEFAULT_PATH}/overview_log_scale.png"

    plot_fname_3 = f"{PLOTS_DIR_DEFAULT_PATH}/overview_bar.png"
    plot_fname_4 = f"{PLOTS_DIR_DEFAULT_PATH}/overview_bar_log_scale.png"

    plot("Overview", plot_fname_1, False, False)
    plot("Overview log scale", plot_fname_2, True, False)

    plot("Overview bar", plot_fname_3, False, True)
    plot("Overview bar log scale", plot_fname_4, True, True)

