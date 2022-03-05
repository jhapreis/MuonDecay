#==================================================
# IMPORTS
#==================================================

import sys
from pathlib import Path

_ = Path().resolve().parent.parent # Add parent.parent folder to sys.path
sys.path.insert(0, str(_))


from source.email.amazon_ses_sample import SendEmail


#==================================================
# SEND EMAIL
#==================================================

if __name__ == '__main__':

    with open("../data/output/output.txt") as f:
        
        msg     = f.read() 
        subject = '[MuonDecay] Acquisition finished'
        SendEmail(subject, msg)
