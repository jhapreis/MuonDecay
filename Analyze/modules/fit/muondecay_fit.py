import ROOT as root

from array import array

import pandas as pd



def MuonDecay_Fit(
    
    root_file_path:   "str" = './results.root',
    
    results_path:     "str" = './results',
    
    tree_name:        "str" = "results",

    branch_name:      "str" = 'time_difference',
            
    numberbins:       "int" = 50
    
    ) -> "int":



    # Open and Read from results.root file
    #----------------------------------------------------------------------------------------------------
    file = root.TFile.Open(root_file_path)

    tree = file.Get(tree_name)

    time_difference = array('f', [0])

    tree.SetBranchAddress(branch_name, time_difference)



    # Create histogram and fit elements
    #----------------------------------------------------------------------------------------------------
    hist    = root.TH1F("hist_delta_t"   , "Time difference", numberbins, 0.8, 9.2 )

    for i in range(tree.GetEntries()):

        tree.GetEntry(i)

        hist.Fill(time_difference[0])


    exp_fit = root.TF1( "exp_fit"        , "[0] + [1]*exp(-x/[2])"      , 0.8, 9.2 )

    exp_fit.SetParNames("constant", "A", "tau")
    exp_fit.SetParameters(10, 100, 2)
    exp_fit.SetLineStyle(2)

    hist.Fit("exp_fit")



    # Retrieve fit parameters
    #----------------------------------------------------------------------------------------------------
    df = pd.DataFrame({
            'constant': [exp_fit.GetParameter(0)],
            'A'       : [exp_fit.GetParameter(1)],
            'tau'     : [exp_fit.GetParameter(2)]
            }).T

    df.reset_index(inplace=True)
    
    df.columns = ['parameter', 'value']

    df.to_csv(results_path+'/expfit.csv')



    # Draw fit on canvas
    #----------------------------------------------------------------------------------------------------
    c1 = root.TCanvas("c1")
    # c1.cd()
    hist.GetXaxis().SetTitle("Time difference #mus")
    hist.GetYaxis().SetTitle("counts")
    hist.Draw()
    c1.SaveAs(results_path+'/expfit.png')



    #----------------------------------------------------------------------------------------------------
    file.Close()

    return 0
