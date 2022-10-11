# Iniciar a aquisição

Com todos os preparativos feitos (vide [este link](../README_PREP.md)), agora basta iniciar a aquisição. Para tanto, é importante lembrar de entrar como usuário `sudo`, usando

```
sudo su
```


Depois que entrar como sudo, acesse a pasta `Acquisition`:

```
cd /MuonDecay/Acquisition
```

e execute o script da aquisição:

```
bash acquisition.sh
```


Se tudo correu bem e está devidamente instalado, os arquivos vão ser re-compilados e a aquisição deve iniciar.

</br>




[2022-10-11] Observação: numa atualização mais recente, foi incorporada uma versão do código em Python que utiliza uma biblioteca do PyVisa para a comunicação com o osciloscópio. Ela ainda precisa da instalação do NI-Visa (assim como a versão C++), possui um tempo de resposta semelhante, mas a diferença está na facilidade de manutenção/modificações, além de possuir recursos mais facilmente incorporados (pelo pyvisa).

Para rodar essa versão, basta se logar como `sudo` e rodar o comando
```
bash AcqPython.sh {DATAFILES_FOLDER} {CONFIG_FILE_PATH} {SCOPE_PARAMETERS_KEY} {ACQUISITION_PARAMETERS_KEY}
```

substituindo os valores conforme o necessário, como por exemplo:

```
bash AcqPython.sh ../Data/raw/ configs/cfg.yaml Scope_Parameters Acquisition_Parameters
```

</br>
</br>
</br>



# Possíveis problemas

</br>



## O compilador não consegue encontrar os headers files (`.h`) do ROOT.

Nesse caso, cabe testar se a execução do ROOT está funcionando pelo usuário sudo. Para testar isso, abra um terminal, entre como sudo e tente rodar o ROOT:

```
sudo su

root
```

Se não for possível encontrar o path do ROOT, siga [esse passo-a-passo](../README_PREP.md#1-instalação-dos-pré-requisitos) para poder acessar o ROOT como sudo.

</br>



## Não foi achado o arquivo `visa.h`

Nesse caso, provavelmente o arquivo não está sendo linkado ao compilador. Para verificar a existência do arquivo, abra um terminal e tente encontrar o arquivo `visa.h`. Normalmente, ele está alocado na pasta `/usr/includes/ni-visa`. Caso seu path esteja diferente, você precisará

* Acessar o arquivo Makefile
* Modificar a variável INCLUDE com o seu path correspondente

Após as modificações, salve o arquivo e tente rodar o programa novamente.

</br>



## O programa é compilado, mas o osciloscópio não é encontrado

Nesse caso, é possível que o acesso não esteja sendo feito como usuário sudo. Lembre-se de rodar o programa da aquisição como super usuário.

Além disso, é possível que o *usbtmc* esteja dando conflitos. Nesse caso, siga [esse passo-a-passo](../README_PREP.md#22-resolução-de-possíveis-problemas) para saber como excluir o driver.

Outra possibilidade é algum problema na instalação do NI-VISA. Nesse caso, deletá-lo e reinstalá-lo pode resolver o problema.



</br>
</br>
</br>





## Sobre o uso do PyVISA

Totalmente válido, porém é importante prestar atenção ao encoder. Para mais informações, verificar [este link](https://docs.python.org/3/library/struct.html).
