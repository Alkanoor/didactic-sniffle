#include <vector>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <sys/wait.h>


int base_pid;

void alarm_handler(int sig)
{
    std::cout<<"Handling"<<std::endl;
    if(signal(SIGALRM, alarm_handler) == SIG_ERR)
    {
        perror("Alarm can't be caught");
        exit(1);
    }
    if(getpid()!=base_pid)
    {
        std::cout<<"We are not in parent"<<std::endl;
        exit(1);
    }
}

int main(int argc, char* argv[])
{
    if(signal(SIGALRM, alarm_handler) == SIG_ERR)
    {
        perror("Alarm can't be caught");
        return -1;
    }

    base_pid = getpid();

    if(argc<4)
    {
        if(argc>=1)
            std::cerr<<"Usage: "<<argv[0]<<" <program> <path to dest> <number of machines to reach>"<<std::endl;
        else
            std::cerr<<"Usage: <program> <path to dest> <number of machines to reach>"<<std::endl;
        return 1;
    }

    std::ifstream conf("addr.conf",std::ios::in);
    if(!conf)
    {
        std::cerr<<"Unable to load configuration file, aborted"<<std::endl;
        return -1;
    }

    std::string temp;
    std::vector<std::string> addrs;
    while(conf>>temp)
        addrs.push_back(temp);

    bool found = false;
    for(unsigned int i=0;i<addrs.size()&&!found;i++)
    {
        std::cout<<"=========="<<addrs[i]<<std::endl;
        int pipe_child[2];
        int pipe_parent[2];
        pipe(pipe_child);
        pipe(pipe_parent);
        int pid = fork();
        if(pid<0)
        {
            perror("Fork failed");
            exit(-1);
        }
        else if(pid==0)
        {
            close(pipe_child[1]);
            close(pipe_parent[0]);
            std::cout<<"Starting"<<std::endl;
            std::cout<<"A"<<std::string("jschoumacher@")+addrs[i]<<"B"<<std::endl;
            std::cout<<(char*)std::string("ssh").c_str()<<std::endl;
            std::string arg1 = "ssh";
            std::string arg2 = "jschoumacher@"+addrs[i];
            char* args[] = {(char*)arg1.c_str(),(char*)arg2.c_str(),"\"whoami\"",NULL};
            std::cout<<"C"<<args[0]<<" "<<args[1]<<"D"<<std::endl;
            dup2(pipe_child[0],STDIN_FILENO);
            dup2(pipe_parent[1],STDOUT_FILENO);
            dup2(pipe_parent[1],STDERR_FILENO);
            if(execv("/usr/bin/ssh",args)<0)
            {
                perror("Execv failed");
                close(pipe_child[0]);
                close(pipe_parent[1]);
            }
        }
        else
        {
            close(pipe_child[0]);
            close(pipe_parent[1]);
            alarm(3);
            std::cout<<"Alarming"<<std::endl;
            char buf[200];
            write(pipe_child[1],"id\n",3);
            read(pipe_parent[0],buf,200);
            for(unsigned int i=0;buf[i]!=0;i++)
                printf("%02x",buf[i]);
            write(pipe_child[1],"whoami\n",3);
            read(pipe_parent[0],buf,200);
            for(unsigned int i=0;buf[i]!=0;i++)
                printf("%02x",buf[i]);
            int status;
            waitpid(pid,&status,0);
            printf("%04x",status);
            for(unsigned int i=0;buf[i]!=0;i++)
              printf("%02x",buf[i]);
            std::cout<<"============"<<std::endl;

            close(pipe_child[1]);
            close(pipe_parent[0]);
        }
    }
}
