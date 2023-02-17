//Code by Caleb Hagens
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "dllist.h"
#include <sys/stat.h>
#include <dirent.h>
#include "fields.h"
#include "jrb.h"
#include <string.h>
// from lecture notes
int compare(Jval v1, Jval v2);           /* Adding a comparison function for inodes. */

void tar(char* direct, JRB tree, int fix);

int main(int argc, char*argv[]){
	JRB inodes;
	inodes = make_jrb();
	struct stat buf;
	int exists, count, i, count2, count3, size, fix;
	count = 0;
	count2 = 0;
	count3 = 0;
	char *prefix, *suffix, *hold, *full, *prev, *a, *buffer;
	//printf("sjnagkjangjkag\n");
	hold = strdup(argv[1]);
	//constructing the prefix string
	for(i = strlen(hold)-1; i > 0; i--)
	{
		
		if(hold[i] == '/')
		{
			i = 0;
		}
		count++;
	}
	fix = strlen(argv[1]) - count +1;
	for(i = strlen(hold)-1; i > 0; i--)
	{
		
		if(hold[i] == '/')
		{
			count3++;
			if(count3 == 2)
			{
				i = 0;
			}
		}
		count2++;
	}
	exists = lstat(argv[1], &buf);
	//set up names after counts
	prefix = malloc(sizeof(char) * (strlen(hold)-count+1));
	prefix = strdup(hold+strlen(hold)-count+1);
	//printf("%s\n",argv[1]);	
	for(i = 0; i < 4; i++)
	{
	//	fprintf(stderr, "%d\n", strlen(cap));
		printf("%c",(strlen(prefix) >> 8*i));
		
	}
	printf("%s",prefix);	
	for(i = 0; i < 8; i++)
	{
		printf("%c", buf.st_ino >> 8*i);
	}
	for(i = 0; i < 4; i++)
	{
		printf("%c",(buf.st_mode >> 8*i));
	}
	for(i = 0; i < 8; i++)
	{
		printf("%c", (buf.st_mtime >> 8*i));
	}


	//fprintf(stderr," %s\n", fix);
	tar(argv[1], inodes, fix);
	//fprintf(stderr, "test\n");


	
}

int compare(Jval v1, Jval v2)           /* Adding a comparison function for inodes. */
{
  if (v1.l < v2.l) return -1;
  if (v1.l > v2.l) return 1;
  return 0;
}



void tar(char* direct, JRB tree, int fix)
{
	DIR *d;
	FILE *fp;
	struct dirent *de;
	struct stat buf;
	Dllist back, dtmp;
	back = new_dllist();
	JRB inodes, tmp;
	Jval jv;
	inodes = make_jrb();
	int exists, count, i, count2, count3, size;
	count = 0;
	count2 = 0;
	count3 = 0;
	long total_size;
	char *prefix, *suffix, *hold, *full, *prev, *buffer, *cap;
	//printf("sjnagkjangjkag\n");
	hold = strdup(direct);
	d = opendir(hold);
	if (d == NULL)
	{
		perror(hold);
		exit(1);
	}
	//constructing the prefix string
	for(i = strlen(hold)-1; i > 0; i--)
	{
		
		if(hold[i] == '/')
		{
			i = 0;
		}
		count++;
	}
	
	for(i = strlen(hold)-1; i > 0; i--)
	{
		
		if(hold[i] == '/')
		{
			count3++;
			if(count3 == 2)
			{
				i = 0;
			}
		}
		count2++;
	}
	//set up names after counts
	prefix = malloc(sizeof(char) * (strlen(hold)));
	prefix = strdup(hold+strlen(hold)-count+1);
	prev = malloc(sizeof(char) * (strlen(hold)));
	prev = strdup(hold+strlen(hold)-count2+1);
	//*********************
	total_size = 0;
	
	//run through given directory
	for(de = readdir(d); de != NULL; de = readdir(d))
	{
		if(strcmp(de->d_name, "..") == 0 || strcmp(de->d_name, ".") == 0)
		{
			continue;
		}
		full = malloc(sizeof(char) * (strlen(hold) + strlen(de->d_name) +10));
		strcpy(full, hold);
		strcat(full, "/");
		strcat(full, de->d_name);
		
		suffix = strdup(de->d_name);
		//***********************
		cap = malloc(sizeof(char) *(strlen(prefix) + 10 + strlen(suffix)));
		strcpy(cap, prefix);
		strcat(cap, "/");
		strcat(cap, suffix);
	
		exists = lstat(full, &buf);
		if(exists < 0)
		{
			exit(1);
		}
		if((buf.st_mode & S_IFMT) == S_IFLNK)
		{
			continue;
		}
		//check inode first
		tmp = jrb_find_int(tree, buf.st_ino);
		if(tmp == NULL)
		{
			jrb_insert_int(tree, buf.st_ino, new_jval_v(NULL));
		}
		//print file size, files name no null, files inode
		//printf("on %s\n", full);
		for(i = 0; i < 4; i++)
		{
		//	fprintf(stderr, "%d\n", strlen(cap));
			printf("%c",(strlen(full+fix) >> 8*i));
			
		}
		//fprintf(stderr, "%d\n%s\n", strlen(full+fix), full+fix);
		printf("%s", full+fix);
		//printf("inode\n");
		for(i = 0; i < 8; i++)
		{
			printf("%c", buf.st_ino >> 8*i);
		}
		//for first time inode
		if(tmp == NULL)
		{
			for(i = 0; i < 4; i++)
			{
				printf("%c",(buf.st_mode >> 8*i));
			}
			for(i = 0; i < 8; i++)
			{
				printf("%c", (buf.st_mtime >> 8*i));
			}
		}
		if(!(buf.st_mode & S_IFDIR))		
		{
			if(tmp == NULL)
			{
				fp = fopen(full, "rb");
				buffer = malloc(sizeof(char) * buf.st_size);
				fread(buffer, buf.st_size, 1, fp);
	
				for(i = 0; i < 8; i++)
				{
					printf("%c", buf.st_size >> 8*i);
				} 
				for(i = 0; i < buf.st_size; i++)
				{
					printf("%c", (int)buffer[i]);
				} 
				fclose(fp);
			}
		}
		else
		{
			dll_append(back, new_jval_s(strdup(full)));
		}
	}
	closedir(d);
	//set up recusion list
	dll_traverse(dtmp, back)
	{
		tar(dtmp->val.s, tree, fix);
	}
}
