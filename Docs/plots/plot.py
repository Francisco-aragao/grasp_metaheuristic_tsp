import re
import matplotlib.pyplot as plt
import sys

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

def plot_average_times(results, output_file):
    tsp_files = list(results.keys())
    avg_times = [results[tsp]["average_time"] for tsp in tsp_files]

    plt.figure(figsize=(10, 6))
    plt.bar(tsp_files, avg_times, color='skyblue')
    plt.xlabel("Instância TSP")
    plt.ylabel("Tempo médio (segundos)")
    plt.title("Tempo médio GRASP - Instâncias TSP")
    plt.xticks(rotation=45, ha="right")
    plt.tight_layout()
    plt.savefig(output_file)
    plt.close()

def parse_file(file_path):
    results = {}
    current_tsp = None

    with open(file_path, "r") as file:
        for line in file:
            # Match TSP file name
            match_tsp = re.search(r"Results for \./tsp_input/EUC_2D/(.+?)\.tsp", line)
            if match_tsp:
                current_tsp = match_tsp.group(1)
                results[current_tsp] = {"times": [], "costs": [], "initial_cost": None}

            # Match VND time
            match_time = re.search(r"VND Time: ([\d.]+) seconds", line)
            if match_time and current_tsp:
                results[current_tsp]["times"].append(float(match_time.group(1)))

            # Match average time
            match_avg_time = re.search(r"Average Elapsed Time: ([\d.]+) seconds", line)
            if match_avg_time and current_tsp:
                results[current_tsp]["average_time"] = float(match_avg_time.group(1))

            # Match average path cost
            match_avg_cost = re.search(r"Average Path Cost: ([\d.]+)", line)
            if match_avg_cost and current_tsp:
                results[current_tsp]["average_cost"] = float(match_avg_cost.group(1))

            # Match initial path cost
            match_initial_cost = re.search(r"Average Initial Path Cost: ([\d.]+)", line)
            if match_initial_cost and current_tsp:
                results[current_tsp]["average_ini_cost"] = float(match_initial_cost.group(1))
            
            # Match initial path cost
            match_best_cost = re.search(r"Best Path Cost: ([\d.]+)", line)
            if match_best_cost and current_tsp:
                results[current_tsp]["best_path"] = float(match_best_cost.group(1))

    return results

def plot_path_difference(results, output_file):
    tsp_files = list(results.keys())
    initial_costs = [results[tsp]["average_ini_cost"] for tsp in tsp_files]  
    avg_costs = [results[tsp]["average_cost"] for tsp in tsp_files]

    # Calculate the percentual difference between initial and final average costs
    percentual_differences = [
        100 * (initial - avg) / initial for initial, avg in zip(initial_costs, avg_costs)
    ]

    # Plot the percentual differences
    plt.figure(figsize=(10, 6))
    plt.bar(tsp_files, percentual_differences, color='lightgreen')
    plt.xlabel("Instância TSP")
    plt.ylabel("Diferença Percentual (%)")
    plt.title("Diferença Percentual entre Custo Médio Inicial e Custo Médio Final")
    plt.xticks(rotation=45, ha="right")
    plt.tight_layout()
    plt.savefig(output_file)
    plt.close()

def plot_solution_comparison(results, best_solutions, output_file):
    tsp_files = list(results.keys())
    avg_costs = [results[tsp]["best_path"] for tsp in tsp_files]
    best_costs = [best_solutions[tsp] for tsp in tsp_files]
    
    relative_errors = [
        100 * (avg - best) / best
        for avg, best in zip(avg_costs, best_costs)
    ]

    plt.figure(figsize=(10, 6))
    plt.bar(tsp_files, relative_errors, color='salmon')
    plt.xlabel("Instância TSP")
    plt.ylabel("Erro relativo (%)")
    plt.title("Erro relativo a solução ótima")
    plt.xticks(rotation=45, ha="right")
    plt.tight_layout()
    plt.savefig(output_file)
    plt.close()

if __name__ == "__main__":
    # get the input file path by command line
    
    file_path = sys.argv[1]
    
    results = parse_file(file_path)

    plot_solution_comparison(results, best_solutions, f'./Docs/plots/solution_comparison_{file_path}.png')
    plot_average_times(results, f'./Docs/plots/average_times_{file_path}.png')
    plot_path_difference(results, f'./Docs/plots/path_difference_{file_path}.png')  # New plot