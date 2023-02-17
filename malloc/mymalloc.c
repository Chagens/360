//Caleb Hagens
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
//look through free list find node big enough for what you want
//ifyou cant find it thats when you call sbrk()

typedef struct chunk{
	int size;
	struct chunk *next;
//	struct chunk *prev;
} *Chunk;

void *my_malloc(size_t size);
void my_free(void *ptr);
void *free_list_begin();
void *free_list_next(void *node);
void coalesce_free_list();

struct chunk *head = NULL;
//function from tutorial point
int comp(const void* a,const void* b)
{
	return (*(int*)a-*(int*)b);
}

void *rm(Chunk prev, Chunk cur)
{
		if(cur->size <= 16)
		{
			if(cur->next != NULL)
			{
				prev->next = cur->next;
			}
			else
			{
				prev->next = NULL;
			}
		}
}

void *my_malloc(size_t size)
{
	Chunk add;
	Chunk tmp;
	Chunk free;
	Chunk prev;
	int bo = 0;
	size = (size + 7 + 8) & -8;
	//initial if no free list
	if(head == NULL)
	{
		if(size > 8192)
		{
			add = (Chunk)sbrk(size);
			add->size = size;
			add->next = NULL;
			return (void*)add+8;
		}
		else
		{
			tmp = (Chunk)sbrk(8192);
			add = (void*)tmp + 8192 - size;
			tmp->size = 8192 - size;
			add->size = size;
			head = tmp;
			if(tmp->size <= 16)
			{
				head = NULL;
			}
			return (void*)add+8;
		}
	}

	tmp = head;
	prev = head;
	
	while(tmp->next != NULL)
	{
		if(tmp->size >= size)
		{
			add = tmp;
			add = (void*)tmp + tmp->size - size;
			tmp->size = tmp->size - size;
			add->size = size;
			rm(prev, tmp);
			return (void*)add+8;
		}
		tmp = tmp->next;
		if(bo == 1)prev = prev->next;
		bo = 1;
	}
	//everything below is to check the last node
	if(tmp != NULL && tmp->size == size)
	{
		add = tmp;
		add = (void*)tmp + tmp->size - size;
		tmp->size = tmp->size - size;
		add->size = size;
		if(bo = 1)
		{
			//only head node
			head = NULL;
		}
		else
		{
			//use prev node
			if(tmp->next != NULL)
			{
				prev->next = tmp->next;
			}
			else
			{
				prev->next = NULL;
			}
		}
		return (void*)add+8; 
	} 
	else if(tmp != NULL && tmp->size < size)
	{
		if(size > 8192)
		{
			add = (Chunk)sbrk(size);
			add->size = size;
			add->next = NULL;
			return (void*)add+8;
		} 
		else
		{	
			free = (Chunk)sbrk(8192);
			free->size = 8192;
			free->next = NULL;
			add = free;
			add = (void*)free + free->size - size;
			free->size = free->size - size;
			add->size = size;
			tmp->next = free;
			rm(tmp, free);
			return (void*)add+8;
		} 
	}
	else if(tmp != NULL && tmp->size > size)
	{
		add = tmp;
		add = (void*)tmp + tmp->size - size;
		tmp->size = tmp->size - size;
		add->size = size;
		return (void*)add+8;
	}
}
void my_free(void *ptr)
{
	if(head == NULL)
	{
		head = ptr-8;
		return;
	}
	ptr = ptr-8;
	Chunk tmp;
	tmp = head;
	while(tmp->next != NULL)
	{
		tmp = tmp->next;
	}
	tmp->next = ptr;
}
void *free_list_begin(){
	return head;
}
void *free_list_next(void *node)
{
	return ((Chunk) node)->next;
}

void coalesce_free_list()
{
	//lets make a malloc and make an array and
	if(head == NULL)
	{
		return;
	}
	Chunk tmp;
	tmp = head;
	int ct = 1;
	//count array then sort using ct
	while(tmp->next != NULL)
	{
		tmp = tmp->next;
		ct++;
	}
	//make array with ct array of ints for cmp function
	int array[ct];
	tmp = head;
	for(int i = 0; i < ct; i++)
	{
		array[i] = (int)tmp;
		tmp = tmp->next;
	}
	//sort array with qsort
	qsort(array, ct, sizeof(int), comp);
	//stack overflow for int and pointer comparison
	//intptr_t got from stackoverflow
	head = (Chunk)(intptr_t)array[0];
	tmp = head;
	for(int i = 1; i < ct; i++)
	{
		tmp->next = (Chunk)(intptr_t)array[i]; 
		tmp = tmp->next;
	}
	tmp->next = NULL;
	tmp = head;
	//merge time Pog
	//if merge then dont go forward
	while(tmp->next != NULL)
	{
		if((void*)tmp + tmp->size == (void*)tmp->next)
		{
			tmp->size += tmp->next->size;
			if(tmp->next->next != NULL)
			{
				tmp->next = tmp->next->next;
			}
			else
			{
				tmp->next = NULL;
			}
		}
		else
		{
			tmp = tmp->next;
	//printf("tmp size free %d \n", ((Chunk) ptr)->size);
		}
		
	}
}
