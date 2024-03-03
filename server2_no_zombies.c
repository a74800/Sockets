/* Um servidor simples no domínio da internet usando TCP
   O número da porta é passado como um argumento 
   Esta versão executa para sempre, criando um processo separado
   para cada conexão
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>

void dostuff(int); /* protótipo da função */
void sigchld_handler(int s); /* protótipo do manipulador de sinal */

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, pid;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;

     // Verifica se o número da porta foi fornecido
     if (argc < 2) {
         fprintf(stderr,"ERRO, nenhuma porta fornecida\n");
         exit(1);
     }
     // Cria um novo socket
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERRO ao abrir o socket");
     // Limpa a estrutura do endereço do servidor
     bzero((char *) &serv_addr, sizeof(serv_addr));
     // Converte a porta fornecida para um número inteiro
     portno = atoi(argv[1]);
     // Configura a estrutura do endereço do servidor
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     // Vincula o socket ao endereço do servidor
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERRO na vinculação");
     // Começa a escutar conexões no socket
     listen(sockfd,5);
     clilen = sizeof(cli_addr);

     // Configura o manipulador de sinal para limpar processos filhos terminados
     struct sigaction sa;
     sa.sa_handler = sigchld_handler; // colhe todos os processos mortos
     sigemptyset(&sa.sa_mask);
     sa.sa_flags = SA_RESTART;
     if (sigaction(SIGCHLD, &sa, NULL) == -1) {
         perror("sigaction");
         exit(1);
     }

     while (1) {
         // Aceita uma nova conexão
         newsockfd = accept(sockfd, 
               (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0) 
             error("ERRO ao aceitar");
         // Cria um novo processo para lidar com a conexão
         pid = fork();
         if (pid < 0)
             error("ERRO na criação do processo");
         if (pid == 0)  {
             // No processo filho, fecha o socket original e lida com a nova conexão
             close(sockfd);
             dostuff(newsockfd);
             exit(0);
         }
         else close(newsockfd); // No processo pai, fecha a nova conexão
     } /* fim do while */
     close(sockfd);
     return 0; /* nunca chegamos aqui */
}

/******** DOSTUFF() *********************
 Há uma instância separada desta função
 para cada conexão. Ela lida com toda a comunicação
 uma vez que uma conexão foi estabelecida.
 *****************************************/
void dostuff (int sock)
{
   int n;
   char buffer[256];
   
   // Limpa o buffer
   bzero(buffer,256);
   // Lê do socket para o buffer
   n = read(sock,buffer,255);
   if (n < 0) error("ERRO ao ler do socket");
   printf("Aqui está a mensagem: %s\n",buffer);
   // Escreve a resposta no socket
   n = write(sock,"Recebi a sua mensagem",22);
   if (n < 0) error("ERRO ao escrever no socket");
}

/******** SIGCHLD_HANDLER() **************
 Esta função é chamada quando um sinal SIGCHLD é recebido,
 o que acontece quando um processo filho termina. Ela
 limpa os processos filhos terminados para evitar processos zumbis.
 ****************************************/
void sigchld_handler(int s)
{
    // Limpa os processos filhos terminados
    while(waitpid(-1, NULL, WNOHANG) > 0);
}
