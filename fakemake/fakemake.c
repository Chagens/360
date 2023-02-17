//Code by Caleb Hagens
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "fields.h"
#include "dllist.h"
#include <sys/stat.h>

void add(Dllist a, char* p);


int main(int argc, char*argv[]){
	//variables
	char* f_name, *hold, *e_name;
	e_name = NULL;
	char B[] =  {"fmakefile"};
	char compile[1000];
	char compile_o[1000];
	char comp_with[1000];
	int i, k, exists, check, check2, check3;
	double max_time_h = 0;
	double max_time_o = 0;
	struct stat buf;
	struct stat buf2;
	IS input;
	Dllist c_files, h_files, f_flags, l_librs, dtmp;
	c_files = new_dllist();
	h_files = new_dllist();
	f_flags = new_dllist();
	l_librs = new_dllist();
	//little check to see what we are gonna call the file
	if(argc == 2)
	{
		f_name = strdup(argv[1]);
	}
	else
	{
		f_name = strdup(B);
	}
	//error check to see if we can even open the file
	input = new_inputstruct(f_name);
	if(input == NULL)
	{
		fprintf(stderr, "no input %s exist\n", f_name);
		return -1;
	}
	//get input
	while(get_line(input) >= 0)
	{
		//ignore blank lines
		//ECHLF input choices
		if(input->NF != 0)
		{
			//c_files read in
			if(strcmp(input->fields[0], "C") == 0 && input->NF > 1)
			{
				for(i = 1; i < input->NF; i++)
				{
					hold = strdup(input->fields[i]);
					add(c_files, hold);
				}
			}
			//h_files read in
			if(strcmp(input->fields[0], "H") == 0  && input->NF > 1)
			{
				for(i = 1; i < input->NF; i++)
				{
					hold = strdup(input->fields[i]);
					add(h_files, hold);
				}
			}
			//libraries read
			if(strcmp(input->fields[0], "L") == 0  && input->NF > 1)
			{
				for(i = 1; i < input->NF; i++)
				{
					hold = strdup(input->fields[i]);
					add(l_librs, hold);
				}
			}
			//read in files
			if(strcmp(input->fields[0], "F") == 0  && input->NF > 1)
			{
				for(i = 1; i < input->NF; i++)
				{
					hold = strdup(input->fields[i]);
					add(f_flags, hold);
				}
			}
			//read in exacutable name
			if(strcmp(input->fields[0], "E") == 0  && input->NF > 1)
			{
				if(e_name != NULL)
				{
					fprintf( stderr, "fmakefile (%d) cannot have more than one E line\n", input->line);
					return -1;
				}
				e_name = strdup(input->fields[1]);
			}
		}
	}
	//error check
	if(e_name == NULL)
	{
		fprintf(stderr, "No executable specified\n");
		return -1;
	}

	//begin calling stat on h_files list
	dll_traverse(dtmp, h_files)
	{
		exists = stat(dtmp->val.s, &buf);
		if(exists < 0)
		{
			fprintf( stderr, "fmakefile: %s: No such file or directory\n", dtmp->val.s);
			return -1;
		}
		else
		{
			if(buf.st_mtime > max_time_h);
			{
				max_time_h = buf.st_mtime;
			}
		}		

	}
	//since we are about to begin compiling lets build the begining of the gcc string
	//make sure clear
	memset(compile, 0, 1000);
	memset(comp_with, 0, 1000);

	strcpy(compile, "gcc -c ");
	//printf("%s\n", compile);
	strcpy(compile_o, "gcc -o ");
	strcat(compile_o, e_name);
	strcat(compile_o, " ");
	dll_traverse(dtmp, f_flags)
	{
		strcat(compile, dtmp->val.s);
		strcat(compile, " ");
	}
	dll_traverse(dtmp, f_flags)
	{
		strcat(compile_o, dtmp->val.s);
		strcat(compile_o, " ");
	}
	//printf("compile string: %s\n", compile);
	//strcat(comp_with, compile);
	//system(compile);
	//printf("compile string: %s\n", compile);
	//begin calling stat on c_files list
	dll_traverse(dtmp, c_files)
	{
		exists = stat(dtmp->val.s, &buf);
		if(exists < 0)
		{
			fprintf( stderr, "fmakefile: %s: No such file or directory\n", dtmp->val.s);
			return -1;
		}
		else
		{
			//call stat on .o files
			//make them .o files
			k = strlen(dtmp->val.s);
			dtmp->val.s[k-1] = 'o';
			exists = stat(dtmp->val.s, &buf2);
			if(exists < 0 && buf2.st_mtime > max_time_o)
			{
				max_time_o = buf2.st_mtime;
			}
			if(exists < 0 || buf2.st_mtime < buf.st_mtime || buf2.st_mtime < max_time_h)
			{
				strcat(compile_o, dtmp->val.s);
				strcat(compile_o, " ");
				dtmp->val.s[k-1] = 'c';
				strcat(comp_with, compile);
				strcat(comp_with, dtmp->val.s);
				printf("%s\n", comp_with);
				if(system(comp_with) != 0)
				{
					fprintf(stderr, "Command failed.  Exiting\n");
					return -1;
				}
				check2 = 1;
				memset(comp_with, 0, strlen(comp_with));
			}
			else
			{
				//check = 1;
			//	printf("%s up to date\n", dtmp->val.s);
				strcat(compile_o, dtmp->val.s);
				strcat(compile_o, " ");
			}
		}	

	}
	//add libraries
	dll_traverse(dtmp, l_librs)
	{
		strcat(compile_o, dtmp->val.s);
		strcat(compile_o, " ");
	}
	//this error checking was too see if up to date getting 5 of the up to dates wrong done know why
	exists = stat(e_name, &buf);
	if(exists < 0)
	{
		check = 0;
	}
	else
	{
		check = 1;
	}
	if(exists < 0)
	{
		if(buf.st_mtime > max_time_o)
		{
			check3 = 1;
		}
		else
		{
			check3 = 0;
		}
	}
	if(check == 0 || check2 == 1 || check3 == 0)
	{
		compile_o[strlen(compile_o)-1] = '\0';
		printf("%s\n", compile_o);
		if(system(compile_o) != 0)
		{
			fprintf(stderr, "Command failed.  Fakemake exiting\n");
			return -1;
		}
	}
	else
	{
		printf("%s up to date\n", e_name);
	}
	//free all memory
	free(f_name);
	free(e_name);
	dll_traverse(dtmp, c_files)
	{
		free(dtmp->val.s);
	}
	dll_traverse(dtmp, h_files)
	{
		free(dtmp->val.s);
	}
	dll_traverse(dtmp, f_flags)
	{
		free(dtmp->val.s);
	}
	dll_traverse(dtmp, l_librs)
	{
		free(dtmp->val.s);
	}

	return 0;
}

void add(Dllist a, char* p)
{
	dll_append(a, new_jval_s(p));
}

