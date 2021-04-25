#!/usr/bin/env python3

import argparse
import matplotlib
import matplotlib.pyplot as plt
import os
import numpy as np

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

if __name__ == "__main__":
    # Read arguments
    parser = argparse.ArgumentParser()

    parser.add_argument("--title", help="Title for the generated plot.")
    parser.add_argument("--no_border", action="store_false", default=True,
                        help=("Do not plot the border between memory "
                              "and compute bounded areas"))
    parser.add_argument("--plot_fname", help="Path of the generated plot.",
                        default=PLOTS_DEFAULT_PATH)
    parser.add_argument("in_files", nargs="+",
                        help=("One or more input files to plot. Input files must "
                             "have the following structure: "
                             "Line 1: peak performance values as val, label; val, label;"
                             "Line 2: bandwidth;"
                             "Line 3: max intensity (for x-axis);"
                             "Line 4-*: dots in the format: dot label, x, y"
                             "the y value can be max(a,b,...), where the arguments "
                             "in the bracket declare which peak performance bounds "
                             "should be considered. Shorthand: max to consider all"))

    args = parser.parse_args()

    in_files        = args.in_files
    plot_fname      = args.plot_fname
    plot_title      = args.title
    do_plot_border  = args.no_border

    fig, ax = plt.subplots()

    for in_file in in_files:
        # Parse log file
        with open(in_file, "r") as in_fp:
            lines = in_fp.readlines()

            pis = []
            pi_labels = []
            for tup in lines[0].split("; "):
                pi, pi_label = tup.split(", ")
                pis.append(literal_eval(pi))
                pi_labels.append(pi_label.rstrip())

            beta    = literal_eval(lines[1])
            maxI    = literal_eval(lines[2])

            dots_x = []
            dots_y = []
            dots_labels = []

            for line in lines[3:]:
                label, x, y = line.split(", ")
                x = literal_eval(x)
                y = y.rstrip()
                label = label.rstrip()

                y_pi_idxs = []
                if "max" in y:
                    if y == "max":
                        y_pi_idxs = list(range(len(pis)))
                    else:
                        y_pi_idxs = list(map(int, y[4:-1].split(",")))

                # Plot every point for every peak performance bound
                if len(y_pi_idxs) > 0:
                    for i in range(len(pis)):
                        if i in y_pi_idxs:
                            # find the limiting bound
                            y_val = min(pis[i], beta * x)
                        else:
                            continue

                        dots_x.append(x)
                        dots_y.append(y_val)
                        dots_labels.append(f"{label} {pi_labels[i]}")
                else:
                    dots_x.append(x)
                    dots_y.append(literal_eval(y))
                    dots_labels.append(label)

        #
        # Create roofline plot
        #

        color_idx = 0

        # Plot bandwidth bound
        xs = np.linspace(2**(-6), maxI, 1000)
        ys = [beta * x for x in xs]
        ax.loglog(xs, ys, linewidth=.8, base=2)
        ax.text(xs[-1] * 0.3, ys[-1] * 1.1, BW_LINE_LABEL,
            fontsize=LABEL_FONT_SIZE, color=colors[color_idx])

        # Plot peak performance bound(s)
        color_idx = (color_idx + 1) % len(colors)
        for i in range(len(pis)):
            xs = np.linspace(0, maxI, 1000)
            ys = [pis[i] for x in xs]
            ax.loglog(xs, ys, linewidth=.8, base=2)
            ax.text(xs[-1]**(1/3), ys[-1] * 1.05, f"{pi_labels[i]}; $P(n) \\leq {pis[i]}$",
                fontsize=LABEL_FONT_SIZE, color=colors[color_idx])

            if do_plot_border:
                # Plot border between memory and compute bounded areas
                intersec_x = pis[i]/beta
                ax.vlines(intersec_x, 0, pis[i], color="gray", linestyle="--", linewidth=.8)
                ax.text(intersec_x * 0.6, 0.3, f"$x = {pis[i]/beta}$",
                        fontsize=LABEL_FONT_SIZE, color="gray")

            color_idx = (color_idx + 1) % len(colors)

        # Plot dots
        markers = ["x", "1", "+", "2", "o", "3"]
        for i in range(len(dots_x)):
            ax.plot(dots_x[i], dots_y[i], marker=markers[i],
                color=colors[color_idx], label=dots_labels[i], linestyle="None")
            color_idx = (color_idx + 1) % len(colors)

        # point_cnt = {}
        # for i, label in enumerate(dots_labels):
        #     # Alternating colors
        #     color = colors[(color_idx + (i//2)) % len(colors)]
        #     ax.plot(dots_x[i], dots_y[i], marker="x", linewidth=.8,
        #         color=color)
        #     point = (dots_x[i], dots_y[i])
        #     point_cnt[point] = point_cnt.get(point, 0) + 1
        #     ax.annotate(label, point, fontsize=LABEL_FONT_SIZE,
        #         xytext=((-1)**(i//(len(dots_labels)//2)) * (2*i + point_cnt[point]) * 5, (-1)**(i+point_cnt[point]) * 30),
        #         textcoords="offset points", color=color,
        #         arrowprops=dict(arrowstyle="->", lw=.8, color=color))

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
