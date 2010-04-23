#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#define CMDCHECK "/tmp/cmd"
#define CMDFILE "/tmp/cmd_a"
#define SYSCMD "cp /tmp/cmd /tmp/cmd_a;rm -f /tmp/cmd"
#define STATFILE "/tmp/stat"
#define CALIBRATE 2
#define RUN 1
#define STOP 0
#define EXIT -1

char cmdline[100];

extern int evtypes[3]; // from main.c

int go_run,run_number;

int check_command()
{
	FILE *fp;
	
	fp = NULL;
	fp = fopen(CMDCHECK,"r");
	if(fp == NULL) return(-1);
	fclose(fp);
	return(1);
}

int write_status()
{
	FILE *fp;
	fp=fopen(STATFILE,"w");
	if(go_run == STOP) fprintf(fp,"No Run");
	else if(go_run == RUN) fprintf(fp,"Running %d %d %d",evtypes[1],evtypes[0],evtypes[2]);
	else if(go_run == CALIBRATE) fprintf(fp,"Calibrate");
	else fprintf(fp,"Bad State");
	fclose(fp);
	return(1);
}

int exec_command()
{
	FILE *fp=NULL;
	char cmd[100];
	
	system(SYSCMD);
	fp = fopen(CMDFILE,"r");
	if(fp == NULL) return(-1);
	if(fgets(cmd,99,fp) != cmd){
		printf("Cannot read command\n");
		fclose(fp);
		return(-1);
	} 
	if(strncmp(cmd,"start_run",strlen("start_run")) == 0){
		if(go_run == STOP){
			sscanf(cmd,"start_run %d",&run_number);
			go_run = RUN;
		}
	}
	if(strncmp(cmd,"calibrate",strlen("calibrate")) == 0){
		if(go_run == STOP) go_run = CALIBRATE;
	}
	if(strncmp(cmd,"exit",strlen("exit")) == 0){
		if(go_run == STOP) go_run = EXIT;
	}
	if(strncmp(cmd,"stop_run",strlen("stop_run")) == 0){
		go_run = STOP;
	}
	if(strncmp(cmd,"status",strlen("status")) == 0){
		write_status();
	}	
	fclose(fp);
	sprintf(cmd,"rm -f %s\n",CMDFILE);
	printf("%s",cmd);
	system(cmd);
	usleep(100);
	return(1);
}
