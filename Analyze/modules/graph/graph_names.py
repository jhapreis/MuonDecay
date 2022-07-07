import ROOT as root

from array import array

from datetime import datetime, timedelta

import seaborn as sns

import matplotlib.pyplot as plt

import pandas as pd

import time



#====================================================================================================
def generate_dates_within_range(start: str, end: str):
    
    if type(start) == str:
        
        start = datetime.strptime(start, "%Y-%m-%d")
    
    if type(end) == str:
        
        end   = datetime.strptime(end  , "%Y-%m-%d")
    
    date_generated = [
        
        datetime.strftime(start + timedelta(days=i), "%Y-%m-%d") \
            for i in range( (end-start).days+1 )  
        ]
    
    dates = pd.DataFrame(date_generated, columns=['time'])
    
    return dates
#====================================================================================================



#====================================================================================================
def incidence_table_daily(df_times, column_name: str='time'):
    
    df_counts = df_times[column_name].value_counts().reset_index()

    df_counts.columns = [column_name, 'counts']
    

    date_range = ( df_counts[column_name].min(), df_counts[column_name].max() )
    
    gen_dates  = generate_dates_within_range(*date_range)
    
    
    df_incidence = df_counts.merge(gen_dates, how='outer').fillna(0).sort_values(by=column_name)
    
    
    return df_incidence
#====================================================================================================



#====================================================================================================
def GraphIncidence_File(

    file_name:      str, 

    folder_path:    str= "./",

    tree_name:      str="tree_waveforms", 
    
    branch_name:    str="names",

    number_of_bins: int=50 
    
    ) -> int:



    #----------------------------------------------------------------------------------------------------
    file = root.TFile.Open(file_name)

    tree = file.Get(tree_name)



    #----------------------------------------------------------------------------------------------------
    name = array('i', [0])

    tree.SetBranchAddress(branch_name, name)

    entries = tree.GetEntries()


    times = []


    for i in range(entries):
        
        tree.GetEntry(i)

        time_as_string = time.strftime(  "%Y-%m-%d", time.localtime(name[0])  )

        times.append(time_as_string)
    
    times = pd.DataFrame(times, columns=['time'])
    
    
    df_incidence = incidence_table_daily(df_times=times, column_name='time')


    # Draw waveforms
    #----------------------------------------------------------------------------------------------------    
    fig, ax = plt.subplots(figsize=(20,20))

    sns.lineplot(x='time', y='counts', data=df_incidence, ax=ax)

    ax.tick_params(rotation=90)

    plt.grid(visible=True)
   
    plt.savefig(folder_path+'/incidence.jpeg')

    plt.clf()


    file.Close()



    return 0
#====================================================================================================
