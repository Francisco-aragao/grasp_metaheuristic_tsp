import re
import matplotlib.pyplot as plt

# Best-known solutions
best_solutions = {
    "pr144": 58537,
    "pr152": 73682,
    "pr124": 59030,
    "st70": 675,
    "pr107": 44303,
    "pr76": 108159,
    "kroE100": 22068,
    "pr136": 96772,
    "kroC100": 20749,
    "rat99": 1211,
    "kroB100": 22141,
    "kroA100": 21282,
    "berlin52": 7542,
    "kroA150": 26524,
    "kroB150": 26130,
    "kroB200": 29437,
    "rat195": 2323,
    "kroA200": 29368,
    "lin105": 14379,
    "kroD100": 21294
}

def parse_file(file_path):
    results = {}
    current_tsp = None

    with open(file_path, "r") as file:
        for line in file:
            # Match TSP file name
            match_tsp = re.search(r"Results for \./tsp_input/EUC_2D/(.+?)\.tsp", line)
            if match_tsp:
                current_tsp = match_tsp.group(1)
                results[current_tsp] = {"times": [], "costs": []}

            # Match VND time
            match_time = re.search(r"VND Time: ([\d.]+) seconds", line)
            if match_time and current_tsp:
                results[current_tsp]["times"].append(float(match_time.group(1)))

            # Match path cost
            match_cost = re.search(r"Final Path Cost: ([\d.]+)", line)
            if match_cost and current_tsp:
                results[current_tsp]["costs"].append(float(match_cost.group(1)))

            # Match average time
            match_avg_time = re.search(r"Average Elapsed Time: ([\d.]+) seconds", line)
            if match_avg_time and current_tsp:
                results[current_tsp]["average_time"] = float(match_avg_time.group(1))

            # Match average path cost
            match_avg_cost = re.search(r"Average Path Cost: ([\d.]+)", line)
            if match_avg_cost and current_tsp:
                results[current_tsp]["average_cost"] = float(match_avg_cost.group(1))

    return results

def plot_average_times(results, output_file):
    tsp_files = list(results.keys())
    avg_times = [results[tsp]["average_time"] for tsp in tsp_files]

    plt.figure(figsize=(10, 6))
    plt.bar(tsp_files, avg_times, color='skyblue')
    plt.xlabel("TSP Instance")
    plt.ylabel("Average Time (seconds)")
    plt.title("Average VND Time per TSP Instance")
    plt.xticks(rotation=45, ha="right")
    plt.tight_layout()
    plt.savefig(output_file)
    plt.close()

def plot_solution_comparison(results, best_solutions, output_file):
    tsp_files = list(results.keys())
    avg_costs = [results[tsp]["average_cost"] for tsp in tsp_files]
    best_costs = [best_solutions[tsp] for tsp in tsp_files]
    
    relative_errors = [
        100 * (avg - best) / best
        for avg, best in zip(avg_costs, best_costs)
    ]

    plt.figure(figsize=(10, 6))
    plt.bar(tsp_files, relative_errors, color='salmon')
    plt.xlabel("TSP Instance")
    plt.ylabel("Relative Error (%)")
    plt.title("Relative Error of Average Cost vs Best Known Solution")
    plt.xticks(rotation=45, ha="right")
    plt.tight_layout()
    plt.savefig(output_file)
    plt.close()

if __name__ == "__main__":
    file_path = "saida_0_0"  # Replace with your file path
    results = parse_file(file_path)

    plot_average_times(results, "average_times.png")
    plot_solution_comparison(results, best_solutions, "solution_comparison.png")