import matplotlib.pyplot as plt
import subprocess
import re


if __name__ == '__main__':
    for generator_it in [10, 14, 18]:
        worker_counts = [1, 2, 4, 8, 16, 24]
        spinlock_points = []
        condvar_points = []
        for worker_count in worker_counts:
            print(f"Benchmark generator_iteration={generator_it} and worker_count={worker_count}")
            result = subprocess.run(
                ['../build/benchmark-cli', '--worker_count', str(worker_count), '--generator_iteration', str(generator_it)],
                stdout=subprocess.PIPE
            )
            output = result.stdout.decode('utf-8')
            spent_time = re.findall(r"\d+", output)
            if len(spent_time) == 2:
                spinlock_points.append(int(spent_time[0]))
                condvar_points.append(int(spent_time[1]))
                print(f"Parsed spent time = {spent_time}")
            else:
                print("No spent time")
        plt.plot(worker_counts, spinlock_points)
        plt.plot(worker_counts, condvar_points)
        plt.xscale('log', base=2)
        plt.xlabel('Worker count')
        plt.ylabel('Time, ms.')
        plt.suptitle(f'Generation iteration = {generator_it}')
        plt.grid(True)
        plt.savefig(f'test_generator_it_{generator_it}.png')
        plt.clf()
