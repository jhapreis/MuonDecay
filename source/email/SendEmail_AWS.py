# Tutorial:
# https://docs.aws.amazon.com/pt_br/ses/latest/DeveloperGuide/send-using-sdk-python.html


#==================================================
# IMPORTS
#==================================================

from botocore.exceptions import ClientError
import boto3



#==================================================
# IMPORT cfg_email.py FILE
#==================================================

CONFIG_FILE_EMAILS = True # it will check the existence of the cfg_email file

try:
    from source.email import cfg_email
except ModuleNotFoundError as e:
    print('cfg_email was not found :/\n\n')
    CONFIG_FILE_EMAILS = False
    


#==================================================
# FUNCTION TO SEND EMAIL (from AWS Simple Email Service)
#==================================================

def SendEmailAWS(subject, msg):

    '''
    Replace sender@example.com with your "From" address.
    This address must be verified with Amazon SES.
    '''
    SENDER = f"mudecay <{cfg_email.EMAIL_ADDRESS}>"


    '''
    Replace recipient@example.com with a "To" address. If your account 
    is still in the sandbox, this address must be verified.
    '''
    RECIPIENT = f"{cfg_email.EMAIL_ADDRESS}"


    '''
    Specify a configuration set. If you do not want to use a configuration
    set, comment the following variable, and the 
    ConfigurationSetName=CONFIGURATION_SET argument below.
    '''
    # CONFIGURATION_SET = "ConfigSet"


    '''
    If necessary, replace us-west-2 with the AWS Region you're using for Amazon SES.
    '''
    AWS_REGION = "us-east-2"


    '''
    The subject line for the email.
    '''
    SUBJECT = subject


    '''
    The email body for recipients with non-HTML email clients.
    '''
    # BODY_TEXT = ("Amazon SES Test (Python)\r\n"
    #             "This email was sent with Amazon SES using the "
    #             "AWS SDK for Python (Boto)."
    #             )
    BODY_TEXT = msg


    '''
    The HTML body of the email.
    '''
    BODY_HTML = """<html>
    <head></head>
    <body>
    <h1>Amazon SES Test (SDK for Python)</h1>
    <p>This email was sent with
        <a href='https://aws.amazon.com/ses/'>Amazon SES</a> using the
        <a href='https://aws.amazon.com/sdk-for-python/'>
        AWS SDK for Python (Boto)</a>.</p>
    </body>
    </html>
                """            


    '''
    The character encoding for the email.
    '''
    CHARSET = "UTF-8"




    '''
    Create a new SES resource and specify a region.
    '''
    client = boto3.client('ses',region_name=AWS_REGION)

    '''
    Try to send the email.
    '''
    try:
        #Provide the contents of the email.
        response = client.send_email(
            Destination={
                'ToAddresses': [
                    RECIPIENT,
                ],
            },
            Message={
                'Body': {
                    'Text': {
                        'Charset': CHARSET,
                        'Data': BODY_TEXT,
                    },
                },
                'Subject': {
                    'Charset': CHARSET,
                    'Data': SUBJECT,
                },
            },
            Source=SENDER,
            # ConfigurationSetName=CONFIGURATION_SET,
        )

    except ClientError as e: # Display an error if something goes wrong.	
        print(e.response['Error']['Message'])
    else:
        print("Email sent! Message ID:"),
        print(response['MessageId'])
