/*
*  lottery.c - Implementacao do algoritmo Lottery Scheduling e sua API
*
*  Autores: SUPER_PROGRAMADORES_C
*  Projeto: Trabalho Pratico I - Sistemas Operacionais
*  Organizacao: Universidade Federal de Juiz de Fora
*  Departamento: Dep. Ciencia da Computacao
*
*
*/

#include "lottery.h"
#include <stdio.h>
#include <string.h>

//Nome unico do algoritmo. Deve ter 4 caracteres.
const char lottName[]="LOTT";
int slot = -1;
//=====Funcoes Auxiliares=====

int ticketsCount(Process *plist){
	int count = 0;
	Process* process = plist;

	while(process != NULL){
		if(processGetSchedSlot(process) ==  slot && (processGetStatus(process) == PROC_READY || processGetStatus(process) == PROC_RUNNING)){
			count += ((LotterySchedParams*) processGetSchedParams(process))->num_tickets;
		}
		process = processGetNext(process);
	}
	return count;
}

//=====Funcoes da API=====

//Funcao chamada pela inicializacao do S.O. para a incializacao do escalonador
//conforme o algoritmo Lottery Scheduling
//Deve envolver a inicializacao de possiveis parametros gerais
//Deve envolver o registro do algoritmo junto ao escalonador
void lottInitSchedInfo() {
	//Cria um ponteiro pra estrutura SchedInfo
	SchedInfo *si = malloc(sizeof(SchedInfo));
 
	// Inicializacao de parametros geraais
	strcpy(si->name, lottName);
	si->initParamsFn = lottInitSchedParams;
	si->scheduleFn = lottSchedule;
	si->releaseParamsFn = lottReleaseParams;
	// Registra o algoritmo junto ao escalonador
	slot = schedRegisterScheduler(si);
}

//Inicializa os parametros de escalonamento de um processo p, chamada 
//normalmente quando o processo e' associado ao slot de Lottery
void lottInitSchedParams(Process *p, void *params) {
	schedSetScheduler(p, params, slot);
}

//Retorna o proximo processo a obter a CPU, conforme o algortimo Lottery 
Process* lottSchedule(Process *plist) {

	Process* process = plist;
	/* Sorteia um ticket  de 1 ao total de tickets */
    int randomTicket = 1 + rand() % ticketsCount(process);

    while(process != NULL){
		if(processGetSchedSlot(process) ==  slot && (processGetStatus(process) == PROC_READY || processGetStatus(process) == PROC_RUNNING)){
			randomTicket -=((LotterySchedParams*) processGetSchedParams(process))->num_tickets;
			if(randomTicket <= 0){
				return process;
			} 
		}
		process = processGetNext(process);
	}
}

//Libera os parametros de escalonamento de um processo p, chamada 
//normalmente quando o processo e' desassociado do slot de Lottery
//Retorna o numero do slot ao qual o processo estava associado
int lottReleaseParams(Process *p) {
	LotterySchedParams *lsp_old = processGetSchedParams(p);
	free(lsp_old);
	return slot;
}

//Transfere certo numero de tickets do processo src para o processo dst.
//Retorna o numero de tickets efetivamente transfeirdos (pode ser menos)
int lottTransferTickets(Process *src, Process *dst, int tickets) {
	LotterySchedParams* srcParams = processGetSchedParams(src);
	LotterySchedParams* dstParams = processGetSchedParams(dst);

	if(srcParams->num_tickets > 0){
		if(srcParams->num_tickets - tickets < 0){
			tickets = srcParams->num_tickets - 1;
		}
			srcParams->num_tickets -= tickets;
			dstParams->num_tickets += tickets;
			return tickets;
	}
	return 0;
}
