
#include "DList.h"

DList * CreateList()
{
	DList * temp = (DList *)malloc(sizeof(struct DList));
	temp->head = NULL;
	temp->tail = NULL;
	temp->len = 0;

	return temp;
}
void DelList(DList * list)
{
	free(list);
}
void InsertList(DList * list, double data)
{
	struct node * temp = (struct node *)malloc(sizeof(struct node));
	temp->data = data;
	if (list->len == 0)
	{
		temp->pre = temp;
		temp->next = temp;
		list->head = temp;
		list->tail = temp;
	}
	else
	{
		temp->pre = list->tail;
		temp->next = list->head;
		list->tail->next = temp;
		list->head->pre = temp;

		list->tail = temp;
	}
	list->len++;
}
void printfall(DList * list)
{
	struct node *temp = list->head;
	for (int i = 0; i < list->len; i++)
	{
		printf("%f\n", temp->data);
		temp = temp->next;
	}
}
void ChangeData(DList * list, double data)
{
	list->head = list->head->next;
	list->tail = list->tail->next;
	list->tail->data = data;

}
void create_list(DList **list, int len, double init_val)
{
	*list = CreateList();
	for (int i = 0; i < len; i++)
		InsertList(*list, init_val);
}
void distroy_list(DList **list)
{
	DelList(*list);
}