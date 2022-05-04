import sys

from modules.fit.muondecay_analysis import MuonDecay_Analysis

from modules.fit.muondecay_fit import MuonDecay_Fit

from modules.graph.graph_results import Graph_Results



if __name__ == "__main__":


    #----------------------------------------------------------------------------------------------------
    if len(sys.argv) <= 1:

        print("Error: you must pass the folder(s) path(es) as parameters when calling the script, but no argument was found.\n")

        exit(1)


    #----------------------------------------------------------------------------------------------------
    for folder in sys.argv[1:]:


        print(f"Folder: {folder}")


        MuonDecay_Analysis(
            root_file_path   = f'{folder}/muondecay/muondecay.root',
            results_path     = f'{folder}/muondecay/results',
            tree_name        = 'tree_waveforms',
            pulsewidth       = 30,
            output_file_path = f'{folder}/muondecay/output.csv'
        )

        MuonDecay_Fit(
            root_file_path = f'{folder}/muondecay/results/results.root',
            results_path   = f'{folder}/muondecay/results',
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
                root_file_path = f'{folder}/muondecay/results/results.root',
                results_path   = f'{folder}/muondecay/results',
                tree_name      = 'results',
                branch_name    = branches[i],
                x_label        = x_labels[i],
                y_label        = 'counts',
                numberbins     = 50
            )
