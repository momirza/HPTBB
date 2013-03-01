from subprocess import Popen, PIPE
from matplotlib import pyplot
import csv
import numpy as np
from scipy import interpolate
import sys



squares = [2**n for n in xrange(11)]

RUNS = 30

def call_executable(square):
    proc_list = []
    get_times = lambda e : map(float, e[0].split())
    for _ in xrange(RUNS):
        proc_list.append(get_times(Popen(['./mat.out', str(square)],stdout=PIPE).stdout.readlines()))
    sums = reduce(lambda x,y: [x[0]+y[0], x[1]+y[1], x[2]+y[2], x[3]+y[3]], proc_list)
    return  map(lambda e: e/RUNS, sums)

def dump_csvs(times):
    """ takes in data as list and generates csv """
    writer = csv.writer(open('mat.csv', 'w'))
    for n, time in enumerate(times):
        writer.writerow([squares[n]] + time)


if __name__ == "__main__":
    DATA_EXISTS = False if sys.argv[-1] == 'rerun' else True

    if DATA_EXISTS:
        reader = csv.reader(open('mat.csv', 'r'))
        times = []
        for row in reader:
            times.append(map(float, row))
        squares, ser, tbb, opt, eli = zip(*times)  
    else:
        times = map(call_executable, squares)
        ser, tbb, opt, eli = zip(*times)
        dump_csvs(times)

    ser_i = interpolate.interp1d(squares, ser)
    tbb_i = interpolate.interp1d(squares, tbb)
    opt_i = interpolate.interp1d(squares, opt)
    eli_i = interpolate.interp1d(squares, eli)
    squaresnew = np.arange(1, max(squares) + 1, 1)


    pyplot.plot(squaresnew, ser_i(squaresnew),'-r', squaresnew, tbb_i(squaresnew),'-b', squaresnew, opt_i(squaresnew),'-k', squaresnew, eli_i(squaresnew),'-g', lw= 1.5)
    pyplot.plot(squares , ser, 'ro', squares, tbb, 'bo', squares, opt, 'ko', squares, eli, 'go')
    pyplot.grid(which='major', axis='both', linestyle='-',color='#C0C0C0', alpha=0.5)
    pyplot.grid(which='minor', axis='y', linestyle='--',color='#C0C0C0', alpha=0.5)
    pyplot.xlabel("n")
    pyplot.legend(('Serial','TBB', 'OPT', 'Elision'), loc=2)
    pyplot.ylabel("Time (s)")
    pyplot.title("Execution Times for matrix-matrix multiplication (%d simulation runs) " % RUNS)
    pyplot.savefig('mat.pdf')
