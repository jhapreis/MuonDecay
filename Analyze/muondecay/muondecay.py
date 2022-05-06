import sys

from pathlib import Path

sys.path.insert(0, str( Path().resolve() ))


from modules.fit.muondecay_analysis import MuonDecay_Analysis

from modules.fit.muondecay_fit import MuonDecay_Fit

from modules.graph.graph_results import Graph_Results



#----------------------------------------------------------------------------------------------------
if len(sys.argv) == 1:
    
    print('You must pass the data folder as an argument.\n\n')
    
    exit(1)

elif len(sys.argv) == 2:
    
    folder_path = f'{sys.argv[1]}/muondecay'
    
    file_name   = folder_path+'/muondecay.root'
        
else:
    
    folder_path = '../Data/muondecay'
    
    file_name   = folder_path+'/muondecay.root'



#----------------------------------------------------------------------------------------------------
print(f"File: {file_name}")


MuonDecay_Analysis(
    root_file_path   = f'{file_name}',
    results_path     = f'{folder_path}/results',
    tree_name        = 'tree_waveforms',
    pulsewidth       = 30,
    output_file_path = f'{folder_path}/output.csv'
)

MuonDecay_Fit(
    root_file_path = f'{folder_path}/results/results.root',
    results_path   = f'{folder_path}/results',
    tree_name      = 'results',
    branch_name    = 'time_difference',
    numberbins     = 50
)


#----------------------------------------------------------------------------------------------------

branches = [
    'x_peak_0',
    'x_peak_1',
    'y_peak_0',
    'y_peak_1',
    'integral_0',
    'integral_1'
]

x_labels = [
    'x_peak_0 (micro-sec)',
    'x_peak_1 (micro-sec)',
    'y_peak_0 (mV)',
    'y_peak_1 (mV)',
    'integrals_0 ()',
    'integrals_1 ()'
]

for i in range(len(branches)):

    Graph_Results(
        root_file_path = f'{folder_path}/results/results.root',
        results_path   = f'{folder_path}/results',
        tree_name      = 'results',
        branch_name    = branches[i],
        x_label        = x_labels[i],
        y_label        = 'counts',
        numberbins     = 50
    )
