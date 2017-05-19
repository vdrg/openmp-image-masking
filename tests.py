import subprocess
import csv

sizes = [3, 5, 8]
threads = [1, 2, 4, 8, 10]

with open('results.csv', 'w') as csvfile:
    fieldnames = ['size', 'threads', 'time']
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)

    writer.writeheader()
    for s in sizes:
        for t in threads:
            time = subprocess.check_output(['./masker', '--no-output', '-s', str(s), '-t', str(t), 'lake.png'])
            print(float(time))
            writer.writerow({'size': s, 'threads': t, 'time': float(time)})
        
