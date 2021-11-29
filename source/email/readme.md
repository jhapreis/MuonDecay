# Envio de emails

Após a aquisição ser finalizada, um script em Python é chamado para mandar um email sobre os resultados da aquisição. 
A aquisição salva os resultados em um arquivo txt e o envio dos email lê desse mesmo txt como mensagem do corpo do email.

<br/>
<br/>

# AWS Simple Email Service

Um cliente da [_AWS Simple Email Service_](https://aws.amazon.com/pt/ses/) é utilizado para enviar os emails. Nesse [link](https://docs.aws.amazon.com/pt_br/ses/latest/DeveloperGuide/send-using-sdk-python.html) você pode encontrar informações mais detalhadas de como preparar o ambiente para o envio dos emails com os SES.

<br/>
<br/>

# Config file dos emails

É necessário possuir um arquivo chamado ```cfg_email.py``` dentro da pasta ```email```. Ele deve ter o seguinte formato:

```

```

Sem esse arquivo, o envio dos emails não irá funcionar.
