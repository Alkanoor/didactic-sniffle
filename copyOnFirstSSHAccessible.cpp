#include <vector>
#include <fstream>
#include <unistd.h>
#include <iostream>


int main(int argc, char* argv[])
{
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
        int pipe_child[2];
        int pipe_parent[2];
        pipe(pipe_child);
        pipe(pipe_parent);
        pid_t pid = fork();
        if(pid<1)
        {
            perror("Fork failed");
            exit(-1);
        }
        else if(pid==0)
        {
            close(pipe_child[0]);
            close(pipe_parent[1]);
            dup2(pipe_child[1],STDIN_FILENO);
            dup2(pipe_parent[0],STDOUT_FILENO);
            char* args[] = {(char*)std::string("ssh").c_str(),(char*)(std::string("jschoumacher@")+addrs[i]).c_str()};
            if(execv("ssh",args)<0)
            {
                perror("Execv failed");
                close(pipe_child[1]);
                close(pipe_parent[0]);
            }
        }
        else
        {
            close(pipe_child[1]);
            close(pipe_parent[0]);
            alarm(3);

            char buf[200];
            write(pipe_child[0],"id",3);
            read(pipe_parent[1],buf,200);
            wait();
            for(unsigned int i=0;buf[i]!=0;i++)
              printf("%02x",buf[i]);
            std::cout<<std::endl;

            close(pipe_child[0]);
            close(pipe_parent[1]);
        }
    }
}
