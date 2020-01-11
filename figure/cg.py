#!/usr/bin/env python3

import matplotlib
import matplotlib.pyplot as plt
import math

if __name__ == "__main__":
    matplotlib.rcParams['pdf.fonttype'] = 42
    matplotlib.rcParams['ps.fonttype'] = 42
    dirbase=''
    figsz = {
        'axes.labelsize': 15,
        'font.size': 15,
        'legend.fontsize': 15,
        'xtick.labelsize': 13,
        'ytick.labelsize': 13,
        'figure.figsize': (6, 4),
    }
    plt.rcParams.update(figsz)

    x = []
    y = []
    with open("../cg/res.txt", "r") as f:
        lines = f.readlines()

    for line in lines[1:]:
        line = line.split()
        x.append(float(line[0]))
        y.append(float(line[1]))

    y0 = []
    y1 = []
    for t in y:
        y0.append(t / 50)
        y1.append(43.92 / t / 7)

    fig, ax0 = plt.subplots()
    ax1 = ax0.twinx()
    plt.plot(x, y0, label="Time")
    plt.plot(x, y1, label="Perf")
    ax0.set_xlabel("Core Number")
    ax0.set_ylabel("Time(s)")
    ax1.set_ylabel("Performance")
    plt.xticks([2 ** t for t in range(len(x))])
    ax0.set_yticks([0.2 * t for t in range(6)])
    ax1.set_yticks([t / 7 for t in range(8)])
    ax0.set_yticklabels([10 * t for t in range(6)])
    ax1.set_yticklabels(range(8))
    plt.xlim(1, 16)
    ax0.set_ylim(0, 1)
    ax1.set_ylim(0, 1)
    plt.legend(loc="upper center", ncol=2, edgecolor='k', shadow=False, framealpha=1)
    fig.tight_layout()
    plt.savefig('cg_scale' + '.png', bbox_inches='tight')
    plt.show()

