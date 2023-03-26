import subprocess
from matplotlib.pyplot import figtext, figure, gca
import toolbox
from matplotlib import pyplot as plt

import tempfile

def testrun(file: str, threads: int, runs: int, iterations: int, m_size: int):
    with tempfile.TemporaryFile() as tempf:
        proc = subprocess.Popen([f"./{file}", f"{threads}", f"{runs}", f"{iterations}", f"{m_size}"], stdout=tempf)
        proc.wait()
        tempf.seek(0)
        data = tempf.read().decode('ASCII')
        data_from_c = data.split(",")
        #Last Item könnte man vielleicht besser im C Programm behandeln, aus Zeitgründen wurde einfach hier der letzte Wert ignoriert
        data_from_c.pop()
        #Erster run wurde als Ausreißer identifiziert und deswegen aus der Auswertung genommen
        data_from_c.pop(0)
        string_to_float = [float(x) for x in data_from_c]
        data_list.append(string_to_float)


runs = 6
run_list = range(1,17)
file_name = 'jacobi_algorithm_modified'
iterations =  [100, 100, 500, 2000, 8000, 50000, 800000]
m_size = [100, 100, 5000, 2500, 1250, 500, 125]

for i in range(len(iterations)):

    data_list = []

    for max_threads in run_list:
        print(f"Run {max_threads}, with {max_threads} threads in {runs} cycles:")
        testrun(file_name, max_threads, runs, iterations[i], m_size[i])

    averagelist = []

    print(data_list)
    for x in data_list:
        averagelist.append(sum(x) / len(x))

    speeduplist = []

    for x in averagelist:
        speeduplist.append(averagelist[0]/ x)

    print(speeduplist)

    plt.plot(run_list, speeduplist, color='green', marker='o', linestyle='solid')

    plt.title(f"Testrun Aufgabe2: Threads:{max_threads}, Measurements:{runs}, \n IterationCount:{iterations[i]}, Matrixsize:{m_size[i]}")
    plt.ylabel("Speedup$")
    plt.xlabel("Threads$")
    plt.show()


figure(figsize=(8, 11), dpi=80)
gca().set_position((.1, .3, .8, .6))
plt.boxplot(data_list)

toolbox.variance(data_list[0])

outputtext = f"STDDEV und DATARANGE über {runs} Durchläufe:\n"

for max_threads in run_list:
    outputtext += f"Messung/Threads {max_threads}: STDDEV : {toolbox.standard_deviation(data_list[max_threads - 1]):.4f}, DATARANGE: {toolbox.data_range(data_list[max_threads - 1]):.4f}\n"

outputtext += f"Die erste Messung jeder Messung/Threads wurde als Ausreißer entfernt"

plt.title(f"Geschwindigkeitsmessung abhängig von benutzen Threads\n 11th Gen Intel(R) Core(TM) i7-11800H @ 2.30GHz\n")
plt.xlabel("Threads")
plt.ylabel("Seconds per run")
figtext(.02, .02, outputtext)
plt.show()
