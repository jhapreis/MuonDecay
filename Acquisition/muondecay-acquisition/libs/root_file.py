import ROOT as root
from array import array


def create_root_file(path: str, arr_waveform_in_units: array, event_name: array):
    
    root_file = root.TFile(path, "CREATE")
    tree_waveforms = root.TTree("tree_waveforms", "waveforms")
    tree_waveforms.Branch("names", event_name  , "name/I")
    tree_waveforms.Branch("waveforms", arr_waveform_in_units, f"waveforms[{len(arr_waveform_in_units)}]/I")
    
    return root_file
