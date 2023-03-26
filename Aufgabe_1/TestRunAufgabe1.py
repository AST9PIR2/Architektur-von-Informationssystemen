import subprocess

from matplotlib.pyplot import figtext, figure, gca

import toolbox
import sys
import tempfile
from collections import Counter
from matplotlib import pyplot as plt
import tempfile


data_list = []

import tempfile

def testrun(threads: int, runs: int):
    with tempfile.TemporaryFile() as tempf:
        proc = subprocess.Popen(["./main", f"{threads}", f"{runs}"], stdout=tempf)
        proc.wait()
        tempf.seek(0)
        data = tempf.read().decode('ASCII')
        data_from_c = data.split(",")
        data_from_c.pop()
        data_from_c.pop(0)
        string_to_float = [float(x) for x in data_from_c]
        data_list.append(string_to_float)

cycles = 20
run_list = range(1,17)

for x in run_list:
    print(f"Run {x}, with {x} threads in {cycles} cycles:")
    testrun(x,cycles)


print(data_list)
figure(figsize=(8, 11), dpi=80)
gca().set_position((.1, .3, .8, .6))
plt.boxplot(data_list)

toolbox.variance(data_list[0])

outputtext = f"STDDEV und DATARANGE über {cycles} Durchläufe:\n"

for x in run_list:
    outputtext += f"Messung/Threads {x}: STDDEV : {toolbox.standard_deviation(data_list[x-1]):.4f}, DATARANGE: {toolbox.data_range(data_list[x-1]):.4f}\n"

outputtext += f"Die erste Messung jeder Messung/Threads wurde als Ausreißer entfernt"

plt.title(f"Geschwindigkeitsmessung abhängig von benutzen Threads\n 11th Gen Intel(R) Core(TM) i7-11800H @ 2.30GHz\n")
plt.xlabel("Threads")
plt.ylabel("Seconds per run")
figtext(.02, .02, outputtext)
plt.show()