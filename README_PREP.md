# Preparação do ambiente de Muon Decay
<br/>





## 1. Instalação dos pré-requisitos




### 1.1. Instalação do python3: 

```
sudo apt install python3-dev
```

<br/>
<br/>



### 1.2. Instalação do ROOT
 
* Build from source: instruções neste [vídeo](https://www.youtube.com/watch?v=QItrmchEQWE&t=184s) do Youtube.

* Se achar melhor, pode baixar [aqui](https://root.cern/install/) a versão de binário do Ubuntu (ou outra distro); assim é mais rápido e mais fácil e, a princípio, traz o mesmo resultado (está feito assim na máquina atual).

* O path atual da instalação do ROOT é
```
~/root/
```

* Se estiver utilizando o VSCode e quiser adicionar ao path do ambiente, basta incluir o path `${ROOTSYS}/include`.

* Importante: executar o ROOT como usuário é simples, basta chamar por `root` no terminal, de qualquer pasta, caso tenha sido adicionado o path ao arquivo `.bashrc`. Porém, quando for realizar a execução pelo usuário `sudo` (root), o chamado do ROOT pode não funcionar. Nesse caso, é necessário adicionar o path do ROOT ao `.bashrc` do usuário sudo também. Para isso, 

    * Abra um terminal e entre como sudo, 

    ```
    sudo su
    ```

   * Acesse o arquivo `.bashrc` e depois adicione o path que está no usuário padrão. Exemplo:

    ```
    nano ~/.bashrc

    *Adicionar o path do ROOT no final do arquivo aberto, substituindo {SEU_NOME_DE_USUARIO} pelo nome correspondente*

    source /home/{SEU_NOME_DE_USUARIO}/root/bin/thisroot.sh
    ``` 

   * Salve e feche o arquivo (ctrl+x, Y, Enter).
   * Feche o terminal e abra outro. Tente rodar o comando `root`. Se tudo ocorreu bem, ele vai abrir um terminal do ROOT.

<br/>
<br/>



### 1.3 Git e GitHub Desktop

Instalação do Git (talvez já esteja até instalado)
```
sudo apt install git
```

<br/>

Para instalar o GitHub Desktop no Ubuntu, veja o passo-a-passo [aqui](https://gist.github.com/berkorbay/6feda478a00b0432d13f1fc0a50467f1).







<br/>
<br/>
<br/>







## 2. Utilizando o NI-VISA

<br/>

A instalação do NI-VISA é feita através do suporte aos drivers dado pela National Instruments. Para instalá-los, baixe o repositório [neste link](https://www.ni.com/pt-br/support/downloads/drivers/download.ni-linux-device-drivers.html#427909) e depois siga as instruções de descompactação e instalação contidas [aqui](https://www.ni.com/pt-br/support/documentation/supplemental/18/downloading-and-installing-ni-driver-software-on-linux-desktop.html).

Nota: tenha certeza de estar baixando a versão mais atual ou a versão que dá suporte ao NI-VISA. Vide esse [link](https://www.ni.com/pdf/manuals/378353e.html) para mais informações. No caso, a versão utilizada atualmente é a versão 2021 Q4.

Após instalar os repositórios, como indicado no [link da instalação](https://www.ni.com/pt-br/support/downloads/drivers/download.ni-linux-device-drivers.html#427909), você pode instalar as ferramentas que você quiser rodando no terminal

```
sudo apt install ni-visa
```

O comando acima é para instalar todos os componentes do NI-VISA, que é, a princípio, só o que será necessário.

<br/>





### 2.1. Execução do LabView

Para executar os softwares do NI-VISA, é possível executar o comando

```
sudo NIvisaic
```

ou 

```
sudo visaconf
```

para exibir o status de conexão do dispositivo. O osciloscópio aparecerá como um instrumento USB.

* Identificação do osciloscópio utilizado:
"USB0::0x0699::0x0363::04WRL8::INSTR"

</br>

![connection](https://user-images.githubusercontent.com/63481188/164304634-08b4dce2-81ee-4c6d-b6e8-db8ed2e59755.jpeg "Foto da janela do visaic.")


</br>

Após isso, acesse o instrumento (clicando sobre ele) e vá até a opção de 
"Input/Output". Lá, você poderá dar um comando de __query__ por __IDN__ e observar a resposta do osciloscópio para saber o que será devolvido. Se tudo estiver funcionando corretamente, o osciloscópio deverá devolver a sua marca e modelo (e outras especificações), como apresentado abaixo.

</br>

![idn](https://user-images.githubusercontent.com/63481188/164304705-c5883241-9968-413c-aa93-81182879f5f7.jpeg "Resultados de uma query por IDN.")


</br>
</br>





### 2.2. Resolução de possíveis problemas



--- 
Sobre as permissões

É importante ter em mente que alguns usuários do mesmo computador podem não ter acesso permitido (ou prioritário) à algumas funcionalidades do sistema operacional. No caso do Linux, rodar os comandos como usuário root (com __sudo__) pode resolver problemas de permissão. 

Rodar o comando 
```
sudo su
```
dará os privilégios de root no terminal sem a necessidade de ficar digitando senha o tempo todo, porém terá o efeito negativo de, caso novos arquivos ou pastas sejam criados, estes pertencerão ao root e não mais ao usuário logado. Portanto, tome cuidado. Para mais informações, visite esse [link](https://www.cyberciti.biz/faq/linux-list-all-members-of-a-group/).

A propósito, é possível saber quem é o dono de determinado arquivo ou diretório rodando o comando 
```
ls -la
```
e se atentando aos nomes que aparecem associados aos arquivos.

</br>



--- 
Sobre a conexão com a USB: para saber se o instrumento é reconhecido pelo sistema operacional via USB, siga as seguintes orientações.

No caso dos sistemas com Linux, é possível listar as conexões USB simplesmente abrindo um terminal e rodando o comando 
```
lsusb
``` 
Ele exibirá as conexões USB do computador. O osciloscópio, caso esteja conectado, aparecerá com seu nome associado, como no exemplo abaixo (olhe em BUS 003 Device 002):

![lsusb](https://user-images.githubusercontent.com/63481188/164304752-91e04fca-8ff7-435a-96c8-f4ae64e84fd0.png "Listagem com o comando lsusb.")

</br>


--- 
Sobre o acesso à porta USB: caso o instrumento USB seja reconhecido pelo sistema operacional, porém não pelo VISA, verifique os passos abaixo.



Cheque se o instrumento está conectado ao computador utilizando o comando mostrado anteriormente (vide nota acima):

```
lsusb
```


No entanto, como dito [aqui](https://knowledge.ni.com/KnowledgeArticleDetails?id=kA00Z0000019KnFSAU&l=pt-BR), por mais que o dispositivo apareça listado, é possível que o VISA não o reconheça. Isso pode acontecer porque algum driver está "se apossando" da USB e não permite que o VISA aja sobre essa porta. No caso, deletar o driver do __usbtmc__ pode ajudar a resolver o problema. 

Abra o terminal e rode

```
rmmod usbtmc
```

Verifique novamente os comandos do 

```
sudo NIvisaic
```

ou 

```
sudo visaconf
```

e veja se agora o dispositivo USB aparece disponível.








<br/>
<br/>
<br/>





## 3. Clonando o repositório

Para obter o código fonte e poder rodar a aquisição, clone o repositório do `MuonDecay` (esse aqui!). No caso do ambiente atual, o repositório foi clonado na pasta `~/Documents`, ficando `~/Documents/MuonDecay`.
