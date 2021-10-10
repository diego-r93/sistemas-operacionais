// Trabalho 2 de Sistemas Operacionais
// Autor: Diego Rodrigues da Silva
// Compilador Utilizado: gcc 9.3.0
// GNU Make 4.2.1
// Sistema Operacional: Ubuntu 20.04.1 LTS

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <stdint.h>
#include <pthread.h>

#define OK 0
#define NUMERO_ARGUMENTOS_INVALIDO 1
#define VALOR_MAXIMO_EXCEDIDO 2
#define ARGUMENTO_INVALIDO 3

#define NUMERO_ARGUMENTOS 2
#define END_OF_STRING '\0'

#define NUM_THREADS 255

uint8_t students = 0;
enum
{
   notHere = 0,
   waiting = 1,
   inTheRoom = 2
} dean;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t turn = PTHREAD_COND_INITIALIZER;
pthread_cond_t clear = PTHREAD_COND_INITIALIZER;
pthread_cond_t lieIn = PTHREAD_COND_INITIALIZER;

void *Dean(void *arg)
{
   pthread_mutex_lock(&mutex);
   while (students > 0 && students < 50)
   {
      dean = waiting;
      pthread_cond_signal(&turn);
      pthread_cond_wait(&lieIn, &mutex);
   }

   while (students >= 50)
   {
      dean = inTheRoom;
      printf("Fim da festa!\n");
      pthread_cond_wait(&clear, &mutex);
   }

   printf("Procurando...\n");
   dean = notHere;
   pthread_mutex_unlock(&mutex);
   return NULL;
}

void *Student(void *arg)
{
   pthread_mutex_lock(&mutex);
   while (dean == inTheRoom)
   {
      pthread_cond_wait(&turn, &mutex);
   }

   students++;

   if (students == 50 && dean == waiting)
   {
      pthread_cond_signal(&lieIn);
   }

   if (students == 0 && dean == waiting)
   {
      pthread_cond_signal(&lieIn);
   }

   else if (students == 0 && dean == inTheRoom)
   {
      pthread_cond_signal(&clear);
   }
   pthread_mutex_unlock(&mutex);
   return NULL;
}

int main(int argc, char *argv[])
{
   uint64_t testeLimite;
   uint8_t numeroDeEstudantes;
   char *verificacao;

   pthread_t threads[NUM_THREADS];
   pthread_t reitor;

   if (argc != NUMERO_ARGUMENTOS)
   {
      printf("Nao foi possivel determinar a quantidade de estudantes.\n");
      printf("Uso: %s <inteiro-nao-negativo>\n", argv[0]);
      exit(NUMERO_ARGUMENTOS_INVALIDO);
   }

   if (argv[1][0] == '-')
   {
      printf("Argumento invalido. Primeiro caractere invalido: \'-\'\n");
      printf("Uso: %s <inteiro-nao-negativo>\n", argv[0]);
      exit(ARGUMENTO_INVALIDO);
   }

   testeLimite = strtoul(argv[1], &verificacao, 10);
   if (*verificacao != END_OF_STRING)
   {
      printf("Argumento invalido. Primeiro caractere invalido: \'%c\'\n", *verificacao);
      exit(ARGUMENTO_INVALIDO);
   }

   if (testeLimite > UCHAR_MAX)
   {
      printf("Valor excede o maximo permitido para unsigned short (%u).\n", UCHAR_MAX);
      exit(VALOR_MAXIMO_EXCEDIDO);
   }

   numeroDeEstudantes = (uint8_t)testeLimite;

   pthread_create(&reitor, NULL, Dean, NULL);
   pthread_join(reitor, NULL);

   for (uint8_t indice = 0; indice < numeroDeEstudantes; indice++)
   {
      pthread_create(&threads[indice], NULL, Student, NULL);
      pthread_join(threads[indice], NULL);
      printf("Numero de estudantes na sala: %d\n", students);
   }

   return OK;
}