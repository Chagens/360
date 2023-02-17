//Caleb Hagens
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "bonding.h"
#include "dllist.h"

struct global_info{
	pthread_mutex_t *lock;
	//put thread id's in dllist
	Dllist H;
	int Hsize;
	Dllist O;
	int Osize;
};

//struct global_info *g;


struct thread_info{
	pthread_cond_t * condition; //to tell if wait or not
	int id;
	int h1;
	int h2;
	int o;
};

void *initialize_v(char *verbosity)
{
	struct global_info * g = (struct global_info *) malloc(sizeof(struct global_info));
	g->H = new_dllist();
	g->Hsize = 0;
	g->O = new_dllist();
	g->Osize = 0;
	g->lock = new_mutex();
	return (void *) g;
}

void *hydrogen(void *arg) //void arg is the bonding_arg
{
	struct thread_info *tmp;
	struct global_info *g;
	struct bonding_arg *bonds = (struct bonding_arg *)arg;
//	printf("Hydrogen thread started %d \n", bonds->id);
	tmp = (struct thread_info *) malloc(sizeof(struct thread_info));
	g = (struct global_info *)bonds->v;
	//set this so when we append it we get all the ids
	//bonds->v = tmp;
	tmp->id = bonds->id;
	tmp->condition = new_cond();
	Dllist a, b;
	struct thread_info *tmph;
	struct thread_info *tmpo;
	char* rv;
	pthread_mutex_lock(g->lock);
	if(!(dll_empty(g->H)) && !(dll_empty(g->O)))
	{
		//there is another hydrogen mol and another oxygen mol
		//can call bond
		//change found threads to match current thread ids
		a = dll_first(g->H);
		b = dll_first(g->O);
		tmph = (struct thread_info *)jval_v(a->val);
		tmpo = (struct thread_info *)jval_v(b->val);
		//set ids for the bond call
		//current thread
		tmp->h1 = tmp->id;
		tmp->h2 = tmph->id;
		tmp->o = tmpo->id;
		//second hydrogen thread
		tmph->h1 = tmp->h1;
		tmph->h2 = tmp->h2;
		tmph->o = tmp->o;
		//on the oxygen thread
		tmpo->h1 = tmp->h1;
		tmpo->h2 = tmp->h2;
		tmpo->o = tmp->o;
		
		dll_delete_node(a);
		dll_delete_node(b);
		g->Hsize--;
		g->Osize--; 

		pthread_cond_signal(tmph->condition);
		pthread_cond_signal(tmpo->condition);
	}
	else
	{
		//append and wait
		dll_append(g->H, new_jval_v(tmp));
		g->Hsize++;
		pthread_cond_wait(tmp->condition, g->lock);
	}
	pthread_mutex_unlock(g->lock);
	rv = Bond(tmp->h1, tmp->h2, tmp->o);
	//call free on condition variables
	//remove from the dllist
	//call free on each thread_info
	//free(tmp);
	//pthread
	pthread_cond_destroy(tmp->condition);
	//free(tmp);
	return rv;

}

void *oxygen(void *arg)
{

	struct thread_info *tmp;
	struct global_info *g;

	struct bonding_arg *bonds = (struct bonding_arg *)arg;
//	printf("Oxygen thread started %d \n", bonds->id);
	tmp = (struct thread_info *) malloc(sizeof(struct thread_info));
	g = (struct global_info *)bonds->v;

	//set this so when we append it we get all the ids
	//bonds->v = tmp;
	tmp->id = bonds->id;
	tmp->condition = new_cond();
	Dllist a, b;
	struct thread_info *tmph;
	struct thread_info *tmph2;
	char* rv;
	pthread_mutex_lock(g->lock);
	if(g->Hsize >= 2 && dll_first(g->H) != dll_last(g->H))
	{
		//there is another hydrogen mol and another oxygen mol
		//can call bond
		//change found threads to match current thread ids
		a = dll_first(g->H);
		b = dll_last(g->H);
		tmph = (struct thread_info *)jval_v(a->val);
		tmph2 = (struct thread_info *)jval_v(b->val);
		//set ids for the bond call
		//current thread
		tmp->o = tmp->id;
		tmp->h1 = tmph->id;
		tmp->h2 = tmph2->id;
		//second hydrogen thread
		tmph->h1 = tmp->h1;
		tmph->h2 = tmp->h2;
		tmph->o = tmp->o;
		//on the second hydrogen thread
		tmph2->h1 = tmp->h1;
		tmph2->h2 = tmp->h2;
		tmph2->o = tmp->o;
		
		dll_delete_node(a);
		dll_delete_node(b);
		g->Hsize--;
		g->Hsize--; 

		pthread_cond_signal(tmph->condition);
		pthread_cond_signal(tmph2->condition);
	}
	else
	{
		//append and wait
		dll_append(g->O, new_jval_v(tmp));
		g->Hsize++;
		pthread_cond_wait(tmp->condition, g->lock);
	}
	pthread_mutex_unlock(g->lock);
	rv = Bond(tmp->h1, tmp->h2, tmp->o);
	//call free on condition variables
	//remove from the dllist
	//call free on each thread_info
//	free(tmp);
	pthread_cond_destroy(tmp->condition);
	//free(tmp);
	return rv;

}
