import matplotlib.pyplot as plt
import subprocess
import re


if __name__ == '__main__':
    for density in [500, 1000, 2000, 4000]:
        points = []
        for threads in [1, 2, 4, 8, 16, 64, 256, 1024]:
            print(f"Benchmark density={density} and threads={threads}")
            result = subprocess.run(
                ['../build/mandelbrot-cli', '--threads', str(threads), '--density', str(density)],
                stdout=subprocess.PIPE
            )
            output = result.stdout.decode('utf-8')
            regex_result = re.search(r"(\d+)", output)
            if regex_result:
                spent_time = int(regex_result.group(1))
                points.append((threads, spent_time))
                print(f"Parsed spent time = {spent_time}")
            else:
                print("No spent time")
        plt.plot(*zip(*points))
        plt.xscale('log')
        plt.xlabel('Log(threads)')
        plt.ylabel('Time, ms.')
        plt.suptitle(f'Density = {density}')
        plt.savefig(f'test_density_{density}.png')
        plt.clf()
