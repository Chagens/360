//code by Caleb Hagens
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "dllist.h"
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include "jrb.h"
#include <sys/time.h>

typedef struct track
{
	char *name;
	int  mode;
	long time;

} Track;

int main(int argc, char*argv[]){
	int     filen_size;
	char    *file_name;
	long    inode;
	int     mode;
	long    mod_time;
	long    file_size;
	char    *bytes;
	FILE    *fp;
	struct timeval T[2];
	Track *PH;
	Track *PHtmp;
	JRB inodes, tmp;
	inodes = make_jrb();

	gettimeofday(&T[0], NULL);
	T[1].tv_usec = 0;
	while(fread(&filen_size, 1, 4, stdin))
	{
		PH = malloc(sizeof(Track));	
		file_name = malloc(sizeof(char) * (filen_size + 10));
		if(fread(file_name, 1, filen_size, stdin) != filen_size)
		{
			fprintf(stderr, "SHEEEEESH\n");
			exit(1);
		}
		file_name[filen_size] = '\0';
		PH->name = strdup(file_name);
		if(fread(&inode, 1, 8, stdin) != 8)
		{
			fprintf(stderr, "SHEEEEESH\n");
			exit(1);
		}
		//jrb for inodes check
		tmp = jrb_find_int(inodes, inode);
		if(tmp == NULL)
		{
			//jrb_insert_int(inodes, inode, new_jval_v(NULL));
			if(fread(&mode, 1, 4, stdin) != 4)
			{
				fprintf(stderr, "SHEEEEESH\n");
				exit(1);
			}
			PH->mode = mode;
			//T[1].tv_sec = mode;
			if(fread(&mod_time, 1, 8, stdin) != 8)
			{
				fprintf(stderr, "SHEEEEESH\n");
				exit(1);
			}
			PH->time = mod_time;
			jrb_insert_int(inodes, inode, new_jval_v((void *)PH));

		}
		else
		{
			//link
			PHtmp = (Track *)tmp->val.v;
			link(PHtmp->name, PH->name);
			continue;
		}
		if(!(mode & S_IFDIR))
		{
			//not a directory open file file_name
			fp = fopen(file_name, "w");
			if(tmp == NULL)
			{
				if(fread(&file_size, 1, 8, stdin) != 8)
				{
					fprintf(stderr, "SHEEEEESH\n");
					exit(1);
				}
				bytes = malloc(sizeof(char) * (file_size));
				if(fread(bytes, 1, file_size, stdin) != file_size)
				{
					fprintf(stderr, "SHEEEEESH\n");
					exit(1);
				}
				fwrite(bytes, 1, file_size, fp);
		
			}
			fclose(fp);

		}
		else
		{
			//so call mkdir on file_name
			mkdir(file_name, S_IRWXU);
		}
	}
	//fix chmod and utimes
	jrb_rtraverse(tmp, inodes)
	{
		PH = (Track *)tmp->val.v;
		chmod(PH->name, PH->mode);
		T[1].tv_sec = PH->time;
		utimes(PH->name, T);
	}
	//free
	jrb_traverse(tmp, inodes)
	{
		PH = (Track *)tmp->val.v;
		free(PH->name);
	}
	jrb_free_tree(inodes);
}
