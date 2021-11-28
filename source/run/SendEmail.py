#==================================================
# IMPORTS
#==================================================

import sys
from pathlib import Path

_ = Path().resolve().parent.parent # Add parent.parent folder to sys.path
sys.path.insert(0, str(_))


from source.email.SendEmail_AWS import CONFIG_FILE_EMAILS, SendEmailAWS



#==================================================
# SEND EMAIL
#==================================================

if __name__ == '__main__':

    if CONFIG_FILE_EMAILS:

        with open("../data/output/output.txt") as f:
            msg     = f.read() 
            subject = '[MuonDecay] Acquisition finished'
            SendEmailAWS(subject, msg)
