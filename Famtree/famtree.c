#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "jrb.h"
#include "fields.h"
#include "dllist.h"

typedef struct person
{
	//default father and mother unkown
	char *name;
	struct person *father;
	struct person *mother;
	//defualt sex is 2 0 is male 1 is female, visited default is 0, childCT default is 0;
	int sex, visited, print, kids; 
	Dllist children;
	
}Person;
//function copied from lecture notes
int is_cycle(Person* n)
{
	Person *n2;
	Dllist dtmp;

	if(n->visited == 1)return 1;
	n->visited = 1;
	dll_traverse(dtmp, n->children)
	{
		n2 = (Person *) dtmp->val.v;
		if(is_cycle(n2)) return 1;
	}
	n->visited = 0;
	return 0;
	
}
	
int parents(Person* p)
{
	if(p->father == NULL && p->mother == NULL)
	{
		//can be printed
		return 1;
	}
	else if(p->father != NULL && p->mother == NULL)
	{
		if(p->father->visited == 1)
		{
			if(parents(p->father) == 1)return 1;
		}
		else
		{
			return 0;
		}
	}
	else if(p->father == NULL && p->mother != NULL)
	{
		if(p->mother->visited == 1)
		{
			if(parents(p->mother) == 1)return 1;
		}
		else
		{
			return 0;
		}	
	}
	else //father and mother do not equal NULL
	{
		if(p->father->visited == 1 && p->mother->visited == 1)
		{
			if(parents(p->father) == 1 && parents(p->mother) == 1 )return 1;
		}
		else
		{
			return 0;
		}
	}
}

int main(int argc, char*argv[]){
	(void)argc;
	Jval jv;
	IS input;
	JRB people, tmp;
	Dllist dtmp;
	people = make_jrb();
	int nsize, jrbSize;
	char *nameHold;
	Person *ptr;
	Person *ptr2;
	Person *current;
	jrbSize = 0;
	//begin read in
	input = new_inputstruct(argv[1]);
	if (input == NULL)
	{
		perror(argv[1]);
		exit(1);
	}
	//read  always starts with persons name then info
	while(get_line(input) >= 0)
	{
		//this sets up the pointer we will be adding information to apply named current 
		//if not person than the ptr will be put into jrb tree
		if(strcmp(input->fields[0], "PERSON") == 0 || strcmp(input->fields[0], "FATHER") == 0 || strcmp(input->fields[0], "MOTHER") == 0 || strcmp(input->fields[0], "FATHER_OF") == 0 || strcmp(input->fields[0], "MOTHER_OF") == 0)
		{
			//first thing we do is set up the name string cause thats a pain
			nsize = strlen(input->fields[1]);
			for(int i = 2; i < input->NF; i++) nsize+= (strlen(input->fields[i])+1);
			//below code is copied from JRB notes for reading in names with multiple spaces
			nameHold = (char *)malloc(sizeof(char)*(nsize+1));
			strcpy(nameHold, input->fields[1]);
			nsize = strlen(input->fields[1]);
			if(input->NF > 2)
			{
				for(int i = 2; i < input->NF; i++)
				{
					nameHold[nsize] = ' ';
					strcpy(nameHold+nsize+1, input->fields[i]);
					nsize += strlen(nameHold+nsize);
				}
			}			
			//set up name code here so we dont have to use hold.s in find
			//father, mother, father of, mother of
			//check if in people struct using tmp
			tmp = jrb_find_str(people, nameHold);
			if(strcmp(input->fields[0], "PERSON") == 0 && input->NF > 1)
			{
				if(tmp == NULL)
				{
					current = malloc(sizeof(Person));
					current->name = strdup(nameHold);
					current->father = NULL;
					current->mother = NULL;
					current->sex = 2;
					current->visited = 0;
					current->children = new_dllist();
					jrb_insert_str(people, current->name, new_jval_v((void *)current));
					jrbSize++;
				}
				else
				{
					current = ((void *)jval_v(tmp->val));
				}
			}
			//now we set up links
			//fathe
			if(strcmp(input->fields[0], "FATHER") == 0 && input->NF > 1)
			{
				//check if in tree
				if(tmp == NULL)
				{
					ptr = malloc(sizeof(Person));
					ptr->name = strdup(nameHold);
					ptr->father = NULL;
					ptr->mother = NULL;
					ptr->children = new_dllist();
					ptr->sex = 0;
					ptr->visited = 0;
					//set up link for children of name
					dll_append(ptr->children, new_jval_v((void *)(current)));
					//check if last person read in already has father
					if(current->father != NULL)
					{
						fprintf( stderr, "Bad Input -- child with two fathers on line %d\n", input->line);
					}
					else
					{
						current->father = ptr;
					}
					jrb_insert_str(people, ptr->name, new_jval_v((void *)ptr));
					jrbSize++;
				}
				else
				{
					if(current->father != NULL)
					{
						fprintf( stderr, "Bad Input -- child with two fathers on line %d\n", input->line);
					}

					((Person *)jval_v(tmp->val))->sex = 0;
					current->father =  ((void *)jval_v(tmp->val));
					dll_append(((Person *)jval_v(tmp->val))->children, new_jval_v((void *)current));
				}
			}
			//mother
			
			if(strcmp(input->fields[0], "MOTHER") == 0 && input->NF > 1)
			{
				//check if in tree
				if(tmp == NULL)
				{
					ptr = malloc(sizeof(Person));
					ptr->name = strdup(nameHold);
					ptr->father = NULL;
					ptr->mother = NULL;
					ptr->children = new_dllist();
					ptr->sex = 1;
					ptr->visited = 0;
					//set up link for children of name
					dll_append(ptr->children, new_jval_v((void *)(current)));
					//check if last person read in already has father
					if(current->mother != NULL)
					{
						fprintf( stderr, "Bad Input -- child with two mothers on line %d\n", input->line);
					}
					else
					{
						current->mother = ptr;
					}
					jrb_insert_str(people, ptr->name, new_jval_v((void *)ptr));
					jrbSize++;
				}
				else
				{
					if(current->mother != NULL)
					{
						fprintf( stderr, "Bad input -- child with two mothers on line %d\n", input->line);
					}

					((Person *)jval_v(tmp->val))->sex = 1;
					current->mother =  ((void *)jval_v(tmp->val));
					dll_append(((Person *)jval_v(tmp->val))->children, new_jval_v((void *)current));
				}		
			} 
			//father of
			if(strcmp(input->fields[0], "FATHER_OF") == 0 && input->NF > 1)
			{
				if(tmp == NULL)
				{
					ptr = malloc(sizeof(Person));
					ptr->name = strdup(nameHold);
					ptr->father = current;
					ptr->mother = NULL;
					ptr->children = new_dllist();
					ptr->sex = 2;
					if(current->sex != 0 && current->sex != 2)
					{
						fprintf( stderr, "Bad input - sex mismatch on line %d\n", input->line);
						return -1;
					}
					current->sex = 0;
					ptr->visited = 0;
					//set up link for children of name
					dll_append(current->children, new_jval_v((void *)(ptr)));
					jrb_insert_str(people, ptr->name, new_jval_v((void *)ptr));
					jrbSize++;
				}
				else
				{
					if(current->sex != 0 && current->sex != 2)
					{
						fprintf( stderr, "Bad input - sex mismatch on line %d\n", input->line);
						return -1;
					}
					current->sex = 0;
					((Person *)jval_v(tmp->val))->father = current;
					dll_append(current->children, new_jval_v((void *)jval_v(tmp->val)));
				}
				
			}
			//Mother of
			if(strcmp(input->fields[0], "MOTHER_OF") == 0 && input->NF > 1)
			{
				if(tmp == NULL)
				{
					ptr = malloc(sizeof(Person));
					ptr->name = strdup(nameHold);
					ptr->father = NULL;
					ptr->mother = current;
					ptr->children = new_dllist();
					ptr->sex = 2;
					if(current->sex != 1 && current->sex != 2)
					{
						fprintf( stderr, "Bad input -- sex mismatch on line %d\n", input->line);
						return -1;
					}
					current->sex = 1;
					current->visited = 0;
					//set up link for children of name
					dll_append(current->children, new_jval_v((void *)(ptr)));
					jrb_insert_str(people, ptr->name, new_jval_v((void *)ptr));
					jrbSize++;
				}
				else
				{
					if(current->sex != 1 && current->sex != 2)
					{
						fprintf( stderr, "Bad input - sex mismatch on line %d\n", input->line);
						return -1;
					}

					current->sex = 1;
					((Person *)jval_v(tmp->val))->mother = current;
					dll_append(current->children, new_jval_v((void *)jval_v(tmp->val)));
				}
				
			}
		}
		//now read in sex
		if(strcmp(input->fields[0], "SEX") == 0 && input->NF > 1)
		{
			/*if(current->sex != 2 || (strcmp(input->fields[1], "M") == 0 && current->sex == 1) || (strcmp(input->fields[1], "F") == 0 && current->sex == 0))
			{
				fprintf( stderr, "Bad input - sex mismatch on line 17\n");
				return -1;
			} */
			if(strcmp(input->fields[1], "M") == 0)
			{
				if(current->sex == 1)
				{
					fprintf( stderr, "Bad input - sex mismatch on line %d\n", input->line);
					return -1;
				}
				current->sex = 0;
			}
			if(strcmp(input->fields[1], "F") == 0)
			{
				if(current->sex == 0)
				{
					fprintf( stderr, "Bad input - sex mismatch on line %d\n", input->line);
					return -1;
				}

				current->sex = 1;
			}
		}
	}
	//all input should be read in at this point
	//so lets call DFS on every node in the tree
	
	jrb_traverse(tmp, people)
	{
		ptr = (Person *) tmp->val.v;
		int k = is_cycle(ptr); 
		if(k == 1)
		{
			fprintf( stderr, "Bad input -- cycle in specification\n");
			return -1;
		}
	}
	//reset visited nodes for the choas of my print statement
	jrb_traverse(tmp, people)
	{
		ptr = (Person *) tmp->val.v;
		ptr->visited = 0;
	}
	//this is the print
	//no topological sort just chaos
	
	int j = 0;
	while(j < jrbSize)
	{
		jrb_traverse(tmp, people)
		{ 
			ptr = (Person *) tmp->val.v;
			if(parents(ptr) == 1 && ptr->visited != 1)
			{
				printf("%s \n", ptr->name);
				if(ptr->sex == 2)
				{
					printf("  Sex: Unknown\n", ptr->sex);
				}
				else if(ptr->sex == 1)
				{
					printf("  Sex: Female\n");
				}
				else
				{
					printf("  Sex: Male\n");
				}
				if(ptr->father != NULL)
				{
					printf("  Father: %s\n", ptr->father->name);
				}
				else
				{
					printf("  Father: Unknown\n");
				} 
				if(ptr->mother != NULL)
				{
					printf("  Mother: %s\n", ptr->mother->name);
				}
				else
				{
					printf("  Mother: Unknown\n");
				}
				if(dll_empty(ptr->children))
				{
					printf("  Children: None\n");
				}
				else
				{
					printf("  Children:\n");
					dll_traverse(dtmp, ptr->children)
					{	
						ptr2 = (Person *) dtmp->val.v;
						printf("    %s\n", ptr2->name); 
					}
				}
				printf("\n");
				ptr->visited = 1;
				j++;
			}
		} 
	}
	//free time
	jettison_inputstruct(input);
	jrb_traverse(tmp, people)
	{
		ptr = (Person *) tmp->val.v;
		free(ptr);
	}

}
