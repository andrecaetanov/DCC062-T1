/*
*  lottery.c - Implementacao do algoritmo Lottery Scheduling e sua API
*
*  Autores: Andre Caetano, Cristiano Nascimento, Lucas Pace
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

//Slot ocupado pelo algoritmo de escalonamento
static int lottSchedSlot;

//=====Funcoes Auxiliares=====

//Retorna o numero total de tickets na lista de processos
int getTotalNumTickets(Process *plist) {
    int totalNumTickets = 0;
    Process *process = plist;
    LotterySchedParams *schedParams = NULL;

    while (process != NULL) {
        if (processGetStatus(process) == PROC_READY || processGetStatus(process) == PROC_RUNNING) {
            schedParams = (LotterySchedParams *) processGetSchedParams(process);
            totalNumTickets += schedParams->num_tickets;
        }

        process = processGetNext(process);
    }

    return totalNumTickets;
}

//Retorna o processo que possui o ticket
Process* getProcessWithTicket(Process *plist, int ticket) {
    Process *processWithTicket = NULL;
    Process *process = plist;
    LotterySchedParams *schedParams = NULL;

    while (process != NULL && ticket > 0) {
        if (processGetStatus(process) == PROC_READY || processGetStatus(process) == PROC_RUNNING) {
            schedParams = (LotterySchedParams *) processGetSchedParams(process);
            ticket -= schedParams->num_tickets;

            if (ticket <= 0) {
                processWithTicket = process;
            }
        }

        process = processGetNext(process);
    }

    return processWithTicket;
}

//=====Funcoes da API=====

//Funcao chamada pela inicializacao do S.O. para a incializacao do escalonador
//conforme o algoritmo Lottery Scheduling
//Deve envolver a inicializacao de possiveis parametros gerais
//Deve envolver o registro do algoritmo junto ao escalonador
void lottInitSchedInfo() {
    SchedInfo *schedInfo = malloc(sizeof(SchedInfo)); //Inicializa o SchedInfo

    //Define os parametros do algoritmo
    strcpy(schedInfo->name, lottName);
    schedInfo->initParamsFn = &lottInitSchedParams;
    schedInfo->scheduleFn = &lottSchedule;
    schedInfo->releaseParamsFn = &lottReleaseParams;

    lottSchedSlot = schedRegisterScheduler(schedInfo); //Registra o algoritmo
}

//Inicializa os parametros de escalonamento de um processo p, chamada
//normalmente quando o processo e' associado ao slot de Lottery
void lottInitSchedParams(Process *p, void *params) {
	processSetSchedParams(p, params);
}

//Retorna o proximo processo a obter a CPU, conforme o algortimo Lottery
Process* lottSchedule(Process *plist) {
    Process *nextProcessGetCpu = NULL;
	int totalNumTickets = getTotalNumTickets(plist);

	if (totalNumTickets > 0) {
	    int chosenTicket = rand() % totalNumTickets + 1;
        nextProcessGetCpu = getProcessWithTicket(plist, chosenTicket);
	}

	return nextProcessGetCpu;
}

//Libera os parametros de escalonamento de um processo p, chamada
//normalmente quando o processo e' desassociado do slot de Lottery
//Retorna o numero do slot ao qual o processo estava associado
int lottReleaseParams(Process *p) {
    LotterySchedParams *schedParams = (LotterySchedParams*) processGetSchedParams(p);

    free(schedParams);

	return lottSchedSlot;
}

//Transfere certo numero de tickets do processo src para o processo dst.
//Retorna o numero de tickets efetivamente transfeirdos (pode ser menos)
int lottTransferTickets(Process *src, Process *dst, int tickets) {
    LotterySchedParams *schedParamsSrc = (LotterySchedParams*) processGetSchedParams(src);
    LotterySchedParams *schedParamsDst = (LotterySchedParams*) processGetSchedParams(dst);
    int ticketsTransferred = 0;

    if (schedParamsSrc->num_tickets >= tickets) {
        ticketsTransferred = tickets;
    }
    else {
        ticketsTransferred = schedParamsSrc->num_tickets - 1;
    }

    schedParamsSrc->num_tickets -= ticketsTransferred;
    schedParamsDst->num_tickets += ticketsTransferred;

	return ticketsTransferred;
}
