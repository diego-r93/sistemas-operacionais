// Trabalho 1 de Sistemas Operacionais
// Autor: Diego Rodrigues da Silva
// Compilador Utilizado: gcc 9.3.0
// GNU Make 4.2.1
// Sistema Operacional: Ubuntu 20.04.1 LTS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

bool getUserCommand(char *, const uint8_t);
void parseString(char *, char **);
int checkCommand(const char *, const char *, int);
void signalEvent(int);

// Variáveis globais utilizadas no tratamento dos sinais
bool _SIGUSR1_ = false;
bool _SIGUSR2_ = false;
bool _SIGTERM_ = false;

int main(void)
{
   // Pode ser alterado para incluir comandos maiores
   const uint8_t tamanhoMaximoDaString = 100 + 2; // /n e /0
   // Diretório padrão dos comandos
   const char *diretorio = "/bin/";

   // Armazenam as informações dos comandos
   // Os nomes já revelam os seus propósitos
   char primeiroComando[tamanhoMaximoDaString];
   char *primeiroComandoFatorado[tamanhoMaximoDaString];
   bool primeiroComandoValido = false;

   char segundoComando[tamanhoMaximoDaString];
   char *segundoComandoFatorado[tamanhoMaximoDaString];
   bool segundoComandoValido = false;

   // Para poder tratar os sinais
   signal(SIGUSR1, signalEvent);
   signal(SIGUSR2, signalEvent);
   signal(SIGTERM, signalEvent);

   // Ajuda muito na hora de buscar o PID do processo
   pid_t pidDoPai = getpid();

   // Recebe os dois comandos, verifica se são válidos,
   // armazena os seus argumentos e checa se eles existem
   while (!primeiroComandoValido)
   {
      printf("Digite o primeiro comando:\n");

      if (!getUserCommand(primeiroComando, tamanhoMaximoDaString))
      {
         printf("O comando digitado excede o tamanho maximo!\n");
         primeiroComandoValido = false;
         continue; // Pula as demais instruções e retorna ao começo do while
      }

      parseString(primeiroComando, primeiroComandoFatorado);

      if (checkCommand(diretorio, primeiroComandoFatorado[0], F_OK) == -1)
      {
         printf("O comando digitado não existe nesse local!\n");
         primeiroComandoValido = false;
      }
      else
      {
         printf("O primeiro comando foi recebido com sucesso!\n");
         primeiroComandoValido = true;
      }
   }

   while (!segundoComandoValido)
   {
      printf("Digite o segundo comando:\n");

      if (!getUserCommand(segundoComando, tamanhoMaximoDaString))
      {
         printf("O comando digitado excede o tamanho maximo!\n");
         segundoComandoValido = false;
         continue; // Pula as demais instruções e retorna ao começo do while
      }

      parseString(segundoComando, segundoComandoFatorado);

      if (checkCommand(diretorio, segundoComandoFatorado[0], F_OK) == -1)
      {
         printf("O comando digitado não existe nesse local!\n");
         segundoComandoValido = false;
      }
      else
      {
         printf("O segundo comando foi recebido com sucesso!\n");
         segundoComandoValido = true;
      }
   }

   printf("\nOs comandos foram recebidos e verificados.\n");
   printf("Aguardando o recebimento de sinais...\n");
   printf("PID do processo pai: %d\n", pidDoPai);

   // Loop infinito para o recebimento dos sinais
   // Somente faz fork() quando recebe um SIGUSR1 ou SIGUSR2
   // Sai do loop quando recebe um SIGTERM
   // Não funciona corretamente se compilado com as flags std=c99 ou std=c11
   // Os argumentos são passados via execvp
   // Referência: https://www.journaldev.com/40793/execvp-function-c-plus-plus
   while (true)
   {
      if (_SIGUSR1_)
      {
         printf("SIGUSR1 Recebido\n");
         pid_t pid = fork();
         if (pid < 0)
         {
            printf("Fork Failed\n");
         }
         else if (pid == 0)
         {
            execvp(primeiroComandoFatorado[0], primeiroComandoFatorado);
            return 0;
         }
         else
         {
            wait(NULL);
            printf("Child Complete\n");
         }
         _SIGUSR1_ = false;
      }

      if (_SIGUSR2_)
      {
         printf("SIGUSR2 Recebido\n");
         pid_t pid = fork();
         if (pid < 0)
         {
            printf("Fork Failed\n");
         }
         else if (pid == 0)
         {
            execvp(segundoComandoFatorado[0], segundoComandoFatorado);
            return 0;
         }
         else
         {
            wait(NULL);
            printf("Child Complete\n");
         }
         _SIGUSR2_ = false;
      }

      if (_SIGTERM_)
      {
         printf("SIGTERM Recebido\n");
         printf("Finalizando o disparador...\n");
         return 0;
      }
   }
}

// Referência: https://www.educative.io/edpresso/how-to-use-the-fgets-function-in-c
// Inspirado em:
// DEITEL,  H. M.; DEITEL, P. J. C Como Programar: 6 ed. São Paulo: Pearson Prentice Hall, 2011, p.268-269
bool getUserCommand(char *returnString, const uint8_t maxStringLength)
{
   fgets(returnString, maxStringLength, stdin);
   if (returnString[strlen(returnString) - 1] != '\n')
   {
      do // Para terminar de ler o buffer stdin
      {
         fgets(returnString, maxStringLength, stdin);
      } while (returnString[strlen(returnString) - 1] != '\n');

      return false;
   }
   else
   {
      returnString[strlen(returnString) - 1] = '\0';
      return true;
   }
}

// Utilizei uma variável temporária pois strtok modifica a string original
// Inspirado em:
// DEITEL,  H. M.; DEITEL, P. J. C Como Programar: 6 ed. São Paulo: Pearson Prentice Hall, 2011, p.279
void parseString(char *inputString, char **parsedString)
{
   char *token;
   char *tempString = inputString;

   uint8_t commandSlice = 0;

   token = strtok(tempString, " ");
   for (; token != NULL; commandSlice++)
   {
      parsedString[commandSlice] = token;
      token = strtok(NULL, " ");
   }
   parsedString[commandSlice] = NULL;
}

// Quando detecta um sinal, seta a sua respectiva variável global
void signalEvent(int sig)
{
   switch (sig)
   {
   case SIGUSR1:
      _SIGUSR1_ = true;
      break;
   case SIGUSR2:
      _SIGUSR2_ = true;
      break;
   case SIGTERM:
      _SIGTERM_ = true;
      break;
   default:
      break;
   }
}

// Recebe o comando, o diretório e o tipo de acesso de acordo com access()
// Usa alocação dinâmica para concatenar o comando e o path
// Os retornos são os mesmos da função access()
// Referências: https://bit.ly/2KMGjNy
// https://www.geeksforgeeks.org/access-command-in-linux-with-examples/
int checkCommand(const char *path, const char *command, int mode)
{
   char *temp;
   int status;

   temp = (char *)malloc((strlen(path) + strlen(command)) * sizeof(char));
   memcpy(temp, path, strlen(path));
   memcpy(temp + strlen(path), command, strlen(command));

   status = access(temp, mode);
   free(temp);
   return status;
}