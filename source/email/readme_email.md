## Envio de emails

Após a aquisição ser finalizada, um script em Python é chamado para mandar um email sobre os resultados da aquisição. 
A aquisição salva os resultados em um arquivo txt e o envio dos email lê desse mesmo txt como mensagem do corpo do email.

Um cliente da _AWS Simple Email Service_ é utilizado para enviar os emails.
