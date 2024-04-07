#include <stdio.h>
#include <sys/types.h>
#include <sys/acct.h>
#include <dirent.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <uthash.h>

#define HASHSIZE 4096
#define CMDLINE_LEN 4096
#define FILENAME_LEN 2048

#define DEFAULT_INTERVAL_SECS 3
#define PERCENT_METRIC_NAME "proc_consumption_percent"
#define PERCENT_METRIC_HELP "HELP "PERCENT_METRIC_NAME" Help\n"
#define PERCENT_METRIC_TYPE "TYPE "PERCENT_METRIC_NAME" gauge\n"
#define PERCENT_METRIC_FORMAT PERCENT_METRIC_NAME"{vm_id=\"%d\",proc_id=\"%d\",cmd=\"%s\"} %d\n"

#define POWER_METRIC_NAME "proc_power"
#define POWER_METRIC_HELP "HELP "POWER_METRIC_NAME" Help\n"
#define POWER_METRIC_TYPE "TYPE "POWER_METRIC_NAME" gauge\n"
#define POWER_METRIC_FORMAT POWER_METRIC_NAME"{vm_id=\"%d\",proc_id=\"%d\",cmd=\"%s\"} %d\n"

typedef struct proc_info{
  int pid, tcpu_ini, tcpu_end, tcpu;
  char name[CMDLINE_LEN];
  UT_hash_handle hh;
};

int get_proc_time(int pid){
  FILE* f;
  char filename[FILENAME_LEN];
  int ucpu=0,scpu=0;
  snprintf(filename, sizeof(filename),  "/proc/%d/stat", pid);

  if ((f = fopen(filename, "r"))){
    fscanf(f, "%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %d %d %*s", &ucpu, &scpu);
  }

  if ((fclose (f))){
    exit(-1);
  }

  return ucpu+scpu;
}

void set_proc_name(int pid, char* n){
  FILE* f;
  char filename[FILENAME_LEN];

  snprintf(filename, sizeof(filename), "/proc/%d/cmdline", pid);

  if ((f = fopen(filename, "r")))
    fread(n,CMDLINE_LEN,1,f);

  if ((fclose (f)))
    exit(-1);

}

void glibc_set_proc_name(int pid, char* n){
  int f;
  char filename[FILENAME_LEN];
  int l=0;

  snprintf(filename, sizeof(filename), "/proc/%d/cmdline", pid);

  if ((f = open(filename, O_RDONLY)))
    l = read(f, n, CMDLINE_LEN);

  for (int i=0; i<l; i++)
    if (n[i]=='\0') n[i]=' ';

  if ((close (f)))
    exit(-1);

}

int main(int argc, char* argv[]) {
  DIR* d;
  struct dirent* cdir;
  int pid;
  struct proc_info *procs, *aux;
  int total_clicks=0;
  procs = NULL;

  unsigned int seconds = DEFAULT_INTERVAL_SECS;
  unsigned int vm_id = 0, power = 0;
  int opt;

  while ((opt = getopt(argc, argv, "hs:m:p:")) != -1) {
    switch (opt){
      case 's': seconds = atoi(optarg);
              break;
      case 'm': vm_id = atoi(optarg);
              break;
      case 'p': power = atoi(optarg);
              break;
      case 'h':
      default: printf ("Usage: %s [shMp]\n", argv[0]);
               printf (" h: this help\n");
               printf (" s [numsecs]: seconds to compute the consumption\n");
               printf (" m [id_vm]: VM ID to show in the metrics\n");
               printf (" p [power]: VM total power consumption in uW\n");

               exit(1);
               break;
    }
  }

  cdir = malloc(sizeof(struct dirent));

  d = opendir("/proc");
  while ((cdir=readdir(d))>0){
    if (sscanf(cdir->d_name, "%d", &pid)){
      aux = malloc(sizeof(struct proc_info));
      aux -> pid = pid;
      aux -> tcpu_ini = get_proc_time(pid);
      HASH_ADD_INT(procs, pid, aux);
    }
  } 
  closedir(d);

  sleep(seconds);

  d = opendir("/proc");
  while ((cdir=readdir(d))>0){
    if (sscanf(cdir->d_name, "%d", &pid)){
      aux = NULL;
      HASH_FIND_INT(procs,&pid,aux);
      if (!aux) {
        aux = malloc(sizeof(struct proc_info));
        aux -> pid = pid;
        aux -> tcpu_ini = 0;
        HASH_ADD_INT(procs, pid, aux);
      }
      aux->tcpu_end = get_proc_time(pid);
      aux->tcpu = abs(aux->tcpu_end - aux->tcpu_ini);
      total_clicks+=aux->tcpu;
    }
  } 
  closedir(d);

  for (aux = procs; aux != NULL; aux = aux->hh.next) {
    if (aux->tcpu){
      glibc_set_proc_name(aux->pid, aux->name);
      if (!power)
        printf(PERCENT_METRIC_FORMAT, vm_id, aux->pid, aux->name, (aux->tcpu*400)/(total_clicks*4));
      else
        printf(POWER_METRIC_FORMAT, vm_id, aux->pid, aux->name, aux->tcpu*(power/total_clicks));
    }
  }

  return (0);
}
