#pragma once

#include <stdlib.h>
#include <stdio.h>

struct node {
	struct node * pre;
	struct node * next;
	double data;
};
struct DList {
	struct node * head;
	struct node * tail;
	int len;
};

DList * CreateList();
void DelList(DList * list);
void InsertList(DList * list, double data);
void ChangeData(DList * list, double data);
void create_list(DList **list, int len, double init_val);
void distroy_list(DList **list);