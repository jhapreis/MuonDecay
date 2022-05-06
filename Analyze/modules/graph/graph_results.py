import ROOT as root

from array import array

import matplotlib.pyplot as plt

import pandas as pd

import seaborn as sns



#====================================================================================================
def Graph_Results(

    root_file_path:   "str" = './results.root',

    results_path:     "str" = './results',

    tree_name:        "str" = 'results',

    branch_name:      "str" = 'y_peak_0',

    x_label:          "str" = 'xlabel',

    y_label:          "str" = 'ylabel',

    numberbins:       "int" = 50

    ) -> "int":


    #----------------------------------------------------------------------------------------------------
    file = root.TFile.Open(root_file_path)

    tree = file.Get(tree_name)

    branch_var = array('f', [0])

    tree.SetBranchAddress(branch_name, branch_var)


    #----------------------------------------------------------------------------------------------------
    values = []

    for i in range(tree.GetEntries()):

        tree.GetEntry(i)

        values.append(branch_var[0])

    values = pd.DataFrame(values, columns=[branch_name])


    #----------------------------------------------------------------------------------------------------
    sns.histplot(
        data   = values,
        bins   = numberbins,
        fill   = True,
        element = 'step'
        )

    plt.xlabel(x_label)

    plt.ylabel(y_label)

    plt.title(branch_name)

    plt.savefig(f"{results_path}/{branch_name}.jpeg", dpi=150)

    plt.clf()

    file.Close()

    return 0
