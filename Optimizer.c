/*
 *********************************************
 *  314 Principles of Programming Languages  *
 *  Fall 2015                                *
 *  Author: Ulrich Kremer                    *
 *  Student Version                          *
 *********************************************
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "InstrUtils.h"
#include "Utils.h"

static void cleanF(Instruction *del);
static void lastOut(Instruction *cur);

static void lastStoreAI(Instruction *cur, int offset);
static void lastASMD(Instruction *cur, int offset);
static void lastLoadI(Instruction *cur, int offset);
static void lastLoadAI(Instruction *cur, int offset);

int main()
{
	Instruction *head;

	head = ReadInstructionList(stdin);
	if (!head) {
		WARNING("No instructions\n");
		exit(EXIT_FAILURE);
	}
	
	head->critical = 1;
	Instruction *cur;
	cur = LastInstruction(head);
	lastOut(cur);
	
	cur = head;
	int offSet;
	
	while(cur != NULL){
		if(cur->opcode == OUTPUTAI){
			cur->critical = 1;
			offSet = cur->field2;
			lastStoreAI(cur->prev, offSet);
		}
		cur = cur->next;
	}
	
	cur = head->next;
	Instruction * last;
	last = head;
	
	while(cur != NULL){
		if(!cur->critical){
			last->next = cur->next;
            free(cur);
            cur = last->next; 
			cur->prev = last;
			continue;
		}
		cur = cur->next;
		last = last->next;
	}
	

	if (head) 
	   PrintInstructionList(stdout, head);
	
	return EXIT_SUCCESS;
}

//------------------------------end of main

static void lastLoadAI(Instruction *cur, int offset){
	if(cur == NULL){
		return;
	}
	
	while(cur != NULL){
		if(cur->opcode == LOADAI && cur->field3 == offset){
			cur->critical = 1;
			lastStoreAI(cur->prev, cur->field2);
		}
		cur = cur->prev;
	}
}

static void lastLoadI(Instruction *cur, int offset){
	if(cur == NULL){
		return;
	}
	while(cur != NULL){
		if(cur->opcode == LOADI && cur->field2 == offset){
			cur->critical = 1;
			return;
		}
		cur = cur->prev;
	}
}

static void lastASMD(Instruction *cur, int offset){
	if(cur == NULL){
		return;
	}
	
	while(cur != NULL){
		if(cur->opcode == ADD || cur->opcode == SUB || cur->opcode == MUL || cur->opcode == DIV){
			if(cur->field3 == offset){
				cur->critical = 1;
				lastASMD(cur->prev, cur->field1);
				lastASMD(cur->prev, cur->field2);
				lastLoadI(cur->prev, cur->field1);
				lastLoadI(cur->prev, cur->field2);
				lastLoadAI(cur->prev, cur->field1);
				lastLoadAI(cur->prev, cur->field2);
				return;
			}
		}
		cur = cur->prev;
	}
}

static void lastStoreAI(Instruction *cur, int offset){
	
	if(cur == NULL){
		return;
	}
	while(cur != NULL){
		if(cur->opcode == STOREAI && cur->field3 == offset){
			cur->critical = 1;
			lastASMD(cur->prev, cur->field1);
			lastLoadI(cur->prev, cur->field1);
			lastLoadAI(cur->prev, cur->field1);
			return;
		}
		cur = cur->prev;
	}
	
}

static void lastOut(Instruction * cur){
	while(cur != NULL){
			if(cur->opcode == OUTPUTAI){
				cur->critical = 1;
				cleanF(cur->next);
				cur->next = NULL;
				return;
			}
			cur = cur->prev;
		} 
}

static void cleanF(Instruction * del){
	
	Instruction * head;
	head = del;
	
	while(head != NULL){
		head = del->next;
		free(del);
		del = head;
	}
}

