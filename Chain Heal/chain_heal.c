//code by: Caleb Hagens
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

typedef struct node
{
	char *name;
	int x, y, visited, cur_PP, max_PP, adj_size, healing;
	struct node *prev;
	struct node **adj;
} Node;

typedef struct inf
{
	int range, jRange, numJumps, best_path_length, best_healing;
	double powerR, power;
	Node **best_path;
	int *hopPower;

}Inf;

int withinRange(int range, int x1, int x2, int y1, int y2)
{
	//function to check if within range ad to adjaceny
	//did not know if c had booleans got error so 1 true 0 false
	//distance formula d = sqrt[(x2-x1)^2+(y2-y1)^2 ]
	int dubx, duby, dubr;
	dubx = (x2 - x1) * (x2 - x1);
	duby = (y2 - y1) * (y2 - y1);
	dubr = range * range;
	if( dubx + duby <= dubr)
	{
		return 1;
	}
	return 0;
}

void DFS(Node* start, Node* from, int hop, Inf *check, int total_healing)
{
	//case to end recursion
	Node *ptr;
	if(check->numJumps == hop)
	{
		return;
	}
	start->visited = 1;
	start->prev = from;
	//2 possible healing healing above his max and not below
	if(start->cur_PP + check->hopPower[hop] > start->max_PP)
	{
		start->healing = check->hopPower[hop];
		total_healing = total_healing + start->max_PP - start->cur_PP;
	}
	else
	{
		start->healing = check->hopPower[hop];
		total_healing = total_healing + check->hopPower[hop];
	}
	
	if(total_healing > check->best_healing)
	{
		check->best_healing = total_healing;
		//traverse prev pointer to get best path
		//hop number size of path
		ptr = start;
		check->best_path_length = hop+1;
		for(int i = hop; i > -1 && ptr != NULL;i--)
		{
			check->best_path[i] = ptr;
			ptr = ptr->prev;
		}
	}

	for(int i = 0; i < start->adj_size; i++)
	{
		if(start->adj[i]->visited != 1)
		{
			DFS(start->adj[i],start, hop + 1, check, total_healing);
		}
	}
	start->visited = 0;

}

int main(int argc, char*argv[]){
	(void)argc;
	int i = 0, range, jRange, numJumps, tmpJ, size;
	double powerR, power, powerHold;
	Node *ptr;
	Node *prev = NULL;
	Node *tmp;
	Node **hold; 
	int x, y, cPP, mPP;
	char name[100];
	Inf *check;
	//command line initial_range, jump_range, num_jumps, initial_power, power_reduction
	//also setting up struct to be passed to DFS function
	check =  malloc(sizeof(Inf)); //call free
	range = atoi(argv[1]);
	jRange = atoi(argv[2]);
	numJumps = atoi(argv[3]);
	power = atoi(argv[4]);
	powerR = atof(argv[5]);

	check->range = range;
	check->jRange = jRange;
	check->numJumps = numJumps;
	check->power = power;
	check->powerR = powerR;
	check->best_healing = 0;
	check->hopPower = malloc(sizeof(int) * numJumps); //call free
	check->best_path = malloc(sizeof(Node) * numJumps); //call free
	powerHold = power;
	for(int j = 0; j < numJumps; j++)
	{
		check->hopPower[j] = rint(powerHold);
		powerHold = powerHold * (1-powerR);
	}

	//read in from text file	
	while(!feof(stdin))
	{
		scanf("%d %d %d %d %s", &x, &y, &cPP, &mPP, name);
		ptr = malloc(sizeof(Node)); //call free
		ptr->x = x;
		ptr->y = y;
		ptr->cur_PP = cPP;
		ptr->max_PP = mPP;
		ptr->name = malloc(sizeof(char) * 100);
		strcpy(ptr->name,name);
		ptr->adj_size = 0;
		ptr->visited = 0;
		ptr->prev = prev;
		i++;
		prev = ptr;
	}
	size = i-1;
	//know amount malloc array
	hold = malloc(sizeof(Node)*i-1); //call free
	tmp = ptr;
	//put list into array the first item is urgosa
	while(tmp != NULL)
	{
		hold[i-1] = tmp;
		i--;
		tmp = tmp->prev;
	}
	//set up adjacency list for nodes
	//iterate through array and set up adjacent
	//first for loop iterates through the array and assigns the adjaceny list for that spot
	//the second for loop starts at the newest item in array due to the last already being checked
	
	for(int j = 0; j < size; j++)
	{
		for(int k = 0; k < size; k++)
		{

			tmpJ = jRange;

			if(withinRange(tmpJ, hold[j]->x, hold[k]->x, hold[j]->y, hold[k]->y) == 1 && k != j)
			{
				//we have compared the numbers they are within either the x range or the y range incrememnt adjaceny list size
				hold[j]->adj_size++;
			}
		}
	}
	//now allocate memory for each adjaceny list
	for(int j = 0; j < size; j++)
	{
		hold[j]->adj =  malloc(sizeof(Node*)*hold[j]->adj_size); //call free
	}
	//now add nodes to the adjacency list using previous loop with extra spot int
	for(int j = 0; j < size; j++)
	{
		int spot = 0;
		for(int k = 0; k < size; k++)
		{
			if(withinRange(tmpJ, hold[j]->x, hold[k]->x, hold[j]->y, hold[k]->y) == 1 && j != k)
			{
				//This is the same as previous nested loop but now i put them into adjacency list
				hold[j]->adj[spot] = hold[k];
				spot++;
			}
		}
	}

	//begin with the DFS call now
	//we call DFS on each node
	for(int j = 0; j < size; j++)
	{
		hold[j]->prev = NULL;
	}
	//within in initial 
	for(int j = 0; j < size; j++)
	{
		if(withinRange(range, hold[0]->x, hold[j]->x, hold[0]->y, hold[j]->y) == 1)
		{	
			DFS(hold[j], hold[0], 0 ,check, 0);
			check->power = power;
		}
	}
	//output for lab
	for(int j = 0; j < check->best_path_length && check->best_path[j] != NULL; j++)
	{
		if(check->best_path[j]->cur_PP + check->hopPower[j] > check->best_path[j]->max_PP)
		{
			check->best_path[j]->healing = check->best_path[j]->max_PP - check->best_path[j]->cur_PP;
		}
		else
		{
			check->best_path[j]->healing = check->hopPower[j];
		}

		printf("%s %d\n", check->best_path[j]->name ,check->best_path[j]->healing);
	}
	printf("Total_Healing %d\n", check->best_healing);
	//begin calling free at this point ************
	free(check->hopPower);
	free(check->best_path);
	free(check);
	for(int j = 0; j <size; j++)
	{
		free(hold[j]->adj);
	}
	for(int j = 0; j <size; j++)
	{
		free(hold[j]->name);
	}
	for(int j = 0; j <size; j++)
	{
		free(hold[j]);
	}
	free(hold);
	return 0;
}
