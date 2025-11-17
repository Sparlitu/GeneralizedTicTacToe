import pandas as pd
import matplotlib.pyplot as plt
import os

CSV_FILE = 'results.csv'
PLOT_DIRECTORY = 'plots'
ALGORITHM_MAP = {1: 'Serial', 2: 'Parallel-V1', 3: 'Parallel-V2'}
COLOR_MAP = {1: 'black', 2: 'blue', 3: 'green'}

def create_plots():
    print(f"Loading data from {CSV_FILE}...")
    try:
        df = pd.read_csv(CSV_FILE)
    except FileNotFoundError:
        print(f"Error: '{CSV_FILE}' not found.")
        print("Please run the './run_experiments.sh' script first to generate the data.")
        return

    if not os.path.exists(PLOT_DIRECTORY):
        os.makedirs(PLOT_DIRECTORY)

    df['Algorithm'] = df['Algorithm'].map(ALGORITHM_MAP)

    serial_times = df[df['Algorithm'] == 'Serial'][['N', 'Depth', 'AvgMoveTime']]
    serial_times = serial_times.rename(columns={'AvgMoveTime': 'SerialTime'})

    df = pd.merge(df, serial_times, on=['N', 'Depth'], how='left')

    df['Speedup'] = df['SerialTime'] / df['AvgMoveTime']

    serial_speedup = serial_times.copy()
    serial_speedup['Algorithm'] = 'Serial'
    serial_speedup['Threads'] = 1
    serial_speedup['Speedup'] = 1.0

    plot_df = pd.concat([df[df['Algorithm'] != 'Serial'], serial_speedup])

    experiment_groups = plot_df.groupby(['N', 'Depth'])

    for (n, depth), group in experiment_groups:
        print(f"Generating plots for N={n}, Depth={depth}...")

        plt.figure(figsize=(10, 6))

        serial_data = df[(df['N'] == n) & (df['Depth'] == depth) & (df['Algorithm'] == 'Serial')]
        if not serial_data.empty:
            serial_time = serial_data['AvgMoveTime'].values[0]
            plt.axhline(y=serial_time, color='black', linestyle=':', linewidth=2, label='Serial')
            all_times_max = max(group['AvgMoveTime'].max(), serial_time)
            plt.ylim(bottom=0, top=all_times_max * 1.2)


        for algo_num, algo_name in ALGORITHM_MAP.items():
            if algo_name == 'Serial': continue
            algo_data = group[group['Algorithm'] == algo_name]
            if not algo_data.empty:
                plt.plot(algo_data['Threads'], algo_data['AvgMoveTime'],
                         marker='o', linestyle='--',
                         label=algo_name, color=COLOR_MAP[algo_num])

        plt.title(f'Average Move Time (N={n}, Depth={depth})')
        plt.xlabel('Number of Threads')
        plt.ylabel('Average Move Time (seconds)')
        plt.legend()
        plt.grid(True, which='both', linestyle=':', linewidth=0.5)
        plt.xticks([1, 2, 4, 8])

        plot_filename = os.path.join(PLOT_DIRECTORY, f'time_N{n}_D{depth}.png')
        plt.savefig(plot_filename)
        plt.close()
        print(f"  Saved time plot to {plot_filename}")

        plt.figure(figsize=(10, 6))

        ideal_threads = [1, 2, 4, 8]
        plt.plot(ideal_threads, ideal_threads,
                 marker='', linestyle=':',
                 label='Ideal (Linear) Speedup', color='red')

        for algo_num, algo_name in ALGORITHM_MAP.items():
            if algo_name == 'Serial': continue

            algo_data = group[group['Algorithm'] == algo_name]
            serial_point = group[(group['Algorithm'] == 'Serial') & (group['Threads'] == 1)]
            algo_data_with_serial = pd.concat([serial_point, algo_data]).sort_values('Threads')

            if not algo_data_with_serial.empty:
                plt.plot(algo_data_with_serial['Threads'], algo_data_with_serial['Speedup'],
                         marker='o', linestyle='-',
                         label=algo_name, color=COLOR_MAP[algo_num])

        plt.title(f'Parallel Speedup (N={n}, Depth={depth})')
        plt.xlabel('Number of Threads')
        plt.ylabel('Speedup (Serial Time / Parallel Time)')
        plt.legend()
        plt.grid(True, which='both', linestyle=':', linewidth=0.5)
        plt.xticks([1, 2, 4, 8])
        plt.yticks(range(1, 9))

        plot_filename = os.path.join(PLOT_DIRECTORY, f'speedup_N{n}_D{depth}.png')
        plt.savefig(plot_filename)
        plt.close()
        print(f"  Saved speedup plot to {plot_filename}")

    print("\nAll plots generated and saved in 'plots' directory.")

if __name__ == "__main__":
    create_plots()