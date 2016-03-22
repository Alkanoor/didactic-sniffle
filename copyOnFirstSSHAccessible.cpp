#include <fstream>
#include <unistd.h>
#include <iostream>


int main(int argc, char* argv[])
{
    if(argc<5)
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
        addrs.pus_back(temp);

    int pipe_child[2];
    pipe(pipe_child);
    pid_t pid = fork();
    if(pid<1)
    {
        perror("Fork failed");
        exit(-1);
    }
    else if(pid==0)
    {
        close(pipe_child[0]);
        dup2(pipe_child[1],STDOUT_FILENO);
        char* args[] = {"ssh","jschoumacher@"}
        execv("ssh",)
    }
    else
    {

    }
}
