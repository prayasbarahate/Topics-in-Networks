#include <iostream>
#include <bits/stdc++.h>
#include <time.h>
#include <fstream>
//#include<vector>
//#include <queue>

using namespace std;
//GLOBAL VARIABLES
int Global_Timer = 0;               //TIME-STEP, AT EVERY TIME STEP , THERE IS PACKET GENERATION , SCHEDULING AND TRANSMISSION WILL TAKE PLACE.
float G_offset = 0.001;             //OFFSET TO BE CONSIDERED TO DECIDE THE FIRST PACKET WHEN THE TIME STEP FOR THE PACKETES IS SAME.
int Total_Packets_Generated =0;     //TOTAL PACKETS IN THE SYSTEM DURING THE WHOLE SIMULATION.
int N;                              //NUMBER OF PORTS.
int buffer_size;                     //BUFFER SIZE
float pg_prob;                       //PROBABILITY OF PACKET GENERATION
int max_timeslots;                   //MAXIMUM TIME SLOTS FOR THE PROCESS SIMULATION.
vector<double> delay;                   //TO KEEP THE TRACK OF PACKETS DELAY FOR CALCULATING AVERAGE DELAY AND STD DEVIATION PER PACKET DELAY.
ofstream fout;
vector<queue<float> > destination;   //TO STORE DESTINATION PORT ID IN INQ BUFFERS
vector<queue<float> > arrival_time;  //TO STORE ARRIVAL TIME OF PACKETS IN INQ BUFFERS
vector<queue<float> > arr_time;      //TO STORE ARRIVAL TIME OF PACKETS IN KOUQ BUFFERS

//PACKET STRUCTURE
struct packet
{
    int time_slot;
    float arrival_offset;
    int dest_port;
};

//TO CREATE INPUT QUEUES FOR INQ
void createInputQueues()
{
    for(int i=0;i<N;i++)
    {
        queue<float> q;
        destination.push_back(q);
        arrival_time.push_back(q);
    }
}

//TO CREATE OUTPUT QUEUES FOR KOUQ
void createOutputQueues()
{
    for(int i=0;i<N;i++)
    {
        queue<float> q;
        arr_time.push_back(q);
    }
}

//INQ SCHEDULING
void INQ(float given_prob,int time)
{
    //CREATE INPUT QUEUES
    createInputQueues();
    double packet_scheduled=0;
    double link_utilisation=0;
    double avg_delay=0;
    //vector<double>delay;
    vector<double> deviation;

    //RUN TILL TIME PROVIDED
    for(int t=1;t<=time;t++)
    {
        //PACKET GENERATION
        for(int inport=0;inport<N;inport++)
        {
            float prob = ((rand()+rand())*rand())%100;
            prob /= 100;
            //CONDITION TO CHECK IF PACKET IS GENERATED OR NOT
            if(prob<given_prob && destination[inport].size()<buffer_size)
            {
                int dest_port = (rand()%N);
                destination[inport].push(dest_port);
                arrival_time[inport].push(t);
                //cout<<"packet generated--> input port:"<<inport<<"  output port:"<<dest_port<<endl;
            }
        }
        //SCHEDULING
        vector<int> check_if_empty(N,0);
        //FOR EACH OUTPUT PORT
        for(int inport=0;inport<N;inport++)
        {
            //CHECK IF PACKETS ARE AVAILABLE FOR THIS OUTPUT PORT
            if(destination[inport].size()!=0)
            {
                //schedule this packet
                int out_port=destination[inport].front();
                //check if this output port is empty
                if(check_if_empty[out_port]==0)
                {
                    //cout<<"packet scheduled--> input port:"<<inport<<"  output port:"<<out_port<<endl;
                    delay.push_back(t-arrival_time[inport].front()+1);
                    packet_scheduled++;
                    link_utilisation++;
                    destination[inport].pop();
                    arrival_time[inport].pop();
                    check_if_empty[out_port]=1;
                }
            }
        }
    }
    //CALCULATE TOTAL DELAY
    for(int i=0;i<delay.size();i++)
        avg_delay=avg_delay+delay[i];
    //DIVIDE TOTAL DELAY BY PACKETS SCHEDULED
    avg_delay=avg_delay/packet_scheduled;
    //CALCULATE LINK UTILISATION
    link_utilisation=link_utilisation/(N*time);
    //CALCULATE STANDARD DEVIATION
    for(int i=0;i<delay.size();i++)
    {
        //STORE SQUARE OF VALUE-MEAN VALUE
        double temp=(delay[i]-avg_delay)*(delay[i]-avg_delay);
        deviation.push_back(temp);
    }
    double sum=0;
    double standard_deviation=0;
    //TOTAL ALL THE SQUARED VALUES
    for(int i=0;i<deviation.size();i++)
    {
        sum+=deviation[i];
    }
    //DIVIDE SUM OF SQUARED(VALUE-MEAN VALUE) BY NUMBER OF VALUES
    sum=(sum/deviation.size());
    standard_deviation=sqrt(sum);
    //PRINT ALL REQUIRED OUTPUTS
    cout<<"INQ:"<<endl;
    cout<<"DELAY:"<<avg_delay<<endl;
    cout<<"LINK UTILISATION:"<<link_utilisation*100<<endl;
    cout<<"STANDARD DEVAITION OF DELAY:"<<standard_deviation<<endl;
    fout << " " <<N << "\t\t" << pg_prob <<"\t\t" << "INQ" <<"\t\t     " << avg_delay <<"\t\t"
                << standard_deviation << "     \t\t" << link_utilisation*100<<endl<<endl<<endl;
    fout.close();
}
void KOUQ(float given_prob,int time,float K_KnockOut)
{
    //CREATE OUTPUT QUEUES
    createOutputQueues();
    //vector<double> delay;
    vector<double> deviation;
    //CALCULATE VALUE OF K
    int K=K_KnockOut*N;
    double drops=0;
    double link_used=0;
    double packets_scheduled=0;
    //RUN TILL MAX TIME
    for(int t=1;t<=time;t++)
    {
        vector<int>counter(N,0);
        //FOR EACH INPUT PORT
        //PACKET GENERATION
        for(int inport=0;inport<N;inport++)
        {
            float prob = ((rand()+rand())*rand())%100;
            prob /= 100;
            //CONDITION TO CHECK IF PACKET IS GENERATED OR NOT
            if(prob<given_prob )
            {
                int dest_port = (rand()%N);
                counter[dest_port]++;
                if(counter[dest_port]>K || arr_time[dest_port].size()>=buffer_size)
                {
                    //packet will be dropped
                    drops++;
                }
                else
                    arr_time[dest_port].push(t+prob);
            }
        }
        //SCHEDULING
        //FOR EACH OUTPUT PORT
        for(int outport=0;outport<N;outport++)
        {
            if(arr_time[outport].size()!=0 )
            {
                delay.push_back(t-arr_time[outport].front()+1);
                arr_time[outport].pop();
                link_used++;
                packets_scheduled++;
            }
        }
    }
    double avg_delay=0;
    //SUM ALL DELAYS
    for(int i=0;i<delay.size();i++)
        avg_delay+=delay[i];
    //CALCULATE AVERAGE DELAY
    avg_delay=avg_delay/packets_scheduled;
    //CALCULATE LINK UTILISATION
    double link_util=link_used/(N*time);
    //CALCULATE DROP PROBABILITY
    double drop_prob=drops/(N*time);
    //CALCULATE STANDARD DEVIATION
    for(int i=0;i<delay.size();i++)
    {
        //STORE SQUARE OF VALUE-MEAN VALUE
        double temp=(delay[i]-avg_delay)*(delay[i]-avg_delay);
        deviation.push_back(temp);
    }
    double sum=0;
    double standard_deviation=0;
     //TOTAL ALL THE SQUARED VALUES
    for(int i=0;i<deviation.size();i++)
    {
        sum+=deviation[i];
    }
     //DIVIDE SUM OF SQUARED(VALUE-MEAN VALUE) BY NUMBER OF VALUES
    sum=(sum/deviation.size());
    //STANDARD DEVIATION OF DELAY
    standard_deviation=sqrt(sum);
    //PRINT ALL REQUIRED OUTPUTS
    cout<<"KOUQ:"<<endl;
    cout<<"DELAY:"<<avg_delay<<endl;
    cout<<"LINK UTILISATION:"<<link_util*100<<endl;
    cout<<"PACKET DROP PROBABILITY:"<<drop_prob<<endl;
    cout<<"STANDARD DEVAITION OF DELAY:"<<standard_deviation<<endl;
    fout << " " <<N << "\t\t" << pg_prob <<"\t\t" << "KOUQ" <<"\t\t     " << avg_delay <<"\t\t"
                << standard_deviation << "     \t\t" << link_util*100<<endl<<endl<<endl;
    fout.close();
}
//FUNCTION TO GENERATE A PACKET WITH A PROBABILITY PG_PROB.
packet* packet_Generation(int port)
{
    //GENERATING A RANDOM NUMBER.
    float prob = ((rand()+rand())*rand())%100;
    prob /= 100;
    struct packet *p = NULL;
    //IF GENERATED NO. IS HAS A PROBABILITY OF GENERATION , GREATER THAN PG_PROB , THEN THE PACKET WILL BE GENERATED.
    if(prob < pg_prob)
    {
        struct packet *p1= new packet();
        int dest_port = (rand()%N);
        float offset = (G_offset)*(rand()%1000)/100;
        p1->time_slot = Global_Timer;
        p1->arrival_offset = offset;
        p1->dest_port = dest_port;
        p = p1;
    }
    return p;
}

//FUNCTION TO COUNT ALL THE USED PORTS OF SWITCH AFTER A SCHEDULING ROUND OF ISLIP.
int Count_used_ports(int accept_arr[])
{
    int count=0;
    for(int i=0; i< N ; i++)
    {
        if(accept_arr[i]!=-1)
            count++;
    }
    return count;
}
//FUNCTION TO DELETE THE TRANSMITTED PACKETS FROM THE QUEUE OF THE PORTS, AS THEY ARE SCHEDULED AND TRANSMITTED.
queue<struct packet*>* delete_packet(queue<struct packet*> q,int dest)
{
    queue<struct packet*> *updated_Q = new queue<struct packet*>();
	while(!q.empty())
    {
        struct packet *p = q.front();
        if(p->dest_port!=dest)
            updated_Q->push(p);
        else
        {
            delay.push_back(Global_Timer - p->time_slot + 1 );
        }
		q.pop();
	}
	return updated_Q;
}
//FUNCTION TO TRAVERSE THE QUEUE OF A PORT.
vector<int> getQueueElements(queue<struct packet*> q)
{
    vector<int> res;
	while(!q.empty())
    {
        struct packet *p = q.front();
		res.push_back(p->dest_port);
		q.pop();
	}
	return res;
}
//FUNCTION TO IMPLEMENT THR GRANT AND ACCEPT PHASE OF ISLIP ITERATIONS BASED ON THE REQUEST GENERATED.
void islip(map<int,vector<int> >& Requests,int* accept_phase)
{
    //GRANT PHASE
    map<int,vector<int> > grant_phase;
    map<int,vector<int> > :: iterator it;
    for(it = Requests.begin();it != Requests.end();it++)
    {
        vector<int> temp = it->second;
        for(int i =0; i < temp.size();i++)
        {
            vector<int> op;
            vector<int> :: iterator iter;
            if(grant_phase.find(temp[i])!= grant_phase.end())
            {
                op = grant_phase[temp[i]];
            }
            iter = find(op.begin(),op.end(),it->first);
            if(iter == op.end())
                op.push_back(it->first);
            grant_phase[temp[i]] = op;
        }
    }
    //ACCEPT PHASE
    int accept_iter[N];     //LOCAL ACCEPT ARRAY TO CONTAIN THE ACCEPT PHASE OUTPUT OF CURRENT ITERATION.
    for(int i=0;i < N ; i++)
        accept_iter[i]=-1;
    //DELETING THE INPUT PORTS FROM THE REQUEST MAP, FOR WHICH ALREADY A OUTPUT REQUEST IS ACCEPTED.
    for(it = grant_phase.begin();it != grant_phase.end();it++)
    {
        vector<int> temp = it->second;
        if(accept_phase[temp.front()]==-1)
        {
            accept_phase[temp.front()] = it->first;
            accept_iter[temp.front()] = it->first;
            Requests.erase(temp.front());
        }
    }
    //DELETING THE OUTPUT PORTS FROM THE REQUEST MAP, SO THAT THEY SHOULD BE CONSIDERED IN THE NEXT ITERATION.
    for(int i=0;i<N;i++)
    {
        if(accept_iter[i] != -1)
        {
            for(it = Requests.begin();it != Requests.end();it++)
            {
                vector<int> temp = it->second;
                temp.erase(std::remove(temp.begin(), temp.end(), accept_iter[i]), temp.end());
                if(temp.size() == 0)
                {
                    Requests.erase(it);
                }
                else
                    it->second=temp;
            }
        }
    }
    //ISLIP ITERATION IS OVER.
}

void call_islip()
{
    //A n SIZE ARRAY TO HOLD THE QUEUES FOR ALL THE PORTS ON THE RESPECTIVE INDICES.
    cout << "Computation is going on , please wait........." <<endl;
    queue<struct packet*>* arr[N];

    for(int i =0; i<N ;i++)
    {
        arr[i] = new queue<struct packet*>();
    }
    map<int, vector<int> > Requests;
    int used_ports =0 ;                 //TAKEN INTO ACCOUNT FOR THE PURPOSE OF CALCULATING LINK UTILIZATION.
    while(max_timeslots--)
    {
        //GENERATION OF PACKET FOR EACH PORT BASED ON PACKET GENERATION PROBABILTIY.
        for(int i=0; i < N ; i++)
        {
            queue<struct packet*> *Q = arr[i];
            struct packet *p1 = new packet();
            p1 = packet_Generation(i);
            if (p1 != NULL && Q->size() < buffer_size)
            {
                Q->push(p1);
                Total_Packets_Generated++;
            }
        }
        //CREATION OF REQUEST FROM THE QUEUES, FOR THE ISLIP.
        for(int i =0; i < N ; i++)
        {
            vector<int> temp;
            queue<struct packet*> *Q = arr[i];
            temp = getQueueElements(*Q);
            if(temp.size())
                Requests[i] = temp;
        }
        //A ACCEPT ARRAY WHICH CONTAINS THE INFO, THAT WHICH OUTPORT ACCECPTED REQUEST FROM WHICH INPUT PORT, FOR THIS CURRENT TIME STEP.
        int accept_phase[N];
        for(int i=0;i < N ; i++)
            accept_phase[i]=-1;

        //WHILE REQUEST SIZE REMAINS NON ZERO, CALL ISLIP TO TRY TO SCHEDULE THE REQUESTS , AS THERE ARE REMAINING INPUT AND OUTPUT PORTS.
        while(Requests.size() > 0 )
        {
            islip(Requests,accept_phase);
        }
        //TO KEEP TRACK OF USED PORTS IN EACH TIME STEP.
        used_ports += Count_used_ports(accept_phase);

        //TRANSMITTING ACCEPTED REQUESTS
        for(int i=0;i < N ; i++)
        {
            if(accept_phase[i] != -1)
            {
                queue<struct packet*> *Q = arr[i];
                arr[i] = delete_packet(*Q,accept_phase[i]);
            }
        }
        //INCREASING THE TIME STEP.
        Global_Timer++;
    }
    //CALCULATING AVERAGE DELAY PER PACKET
    float av_delay = 0;
    for(int i=0; i < delay.size() ; i++)
    {
        av_delay += delay[i];
    }
    av_delay /= delay.size();
    //CALCULATING STD DEVIATION PER PACKET DELAY.
    float std_delay=0;
    for(int i=0; i < delay.size() ; i++)
    {
        delay[i] -= av_delay;
        delay[i] *= delay[i];
        std_delay += delay[i];
    }
    std_delay /= delay.size();
    std_delay = sqrt(std_delay);

    //PRINT ALL REQUIRED OUTPUTS
    cout<<"ISLIP:"<<endl;
    cout<<"DELAY:"<<av_delay<<endl;
    cout<<"LINK UTILISATION:"<<used_ports*100.0/(N*Global_Timer) << endl;
    cout<<"STANDARD DEVAITION OF DELAY:"<<std_delay<<endl;
    //OUTPUT TO THE FILE.
    fout << " " <<N << "\t\t" << pg_prob <<"\t\t" << "ISLIP" <<"\t\t     " << av_delay <<"\t\t"
                << std_delay << "     \t\t" << used_ports*100.0/(N*Global_Timer)<<endl<<endl<<endl;
    fout.close();
}

//MAIN FUNCTION TO CALL THE RESPECTIVE ALGORITHM FUNCTIONS.
int main(int argc, char* argv[])
{
    //DEFAULT VALUES FOR PARAMETERS......
    N = 8;
    buffer_size = 4;
    pg_prob = 0.5;
    string Queue = "inq";
    float K_KnockOut = 0.6;
    string fileName = "Output.txt";
    max_timeslots = 10000;
    int flag=1,curr=1;
    //READING PARAMETERS VALUES FROM COMMAND LINE...........
    while(curr<argc)
    {
        if(curr+2>=argc+1)
        {
            flag=0;
            break;
        }
        if(strcmp(argv[curr],"-N")==0)
            N=atoi(argv[curr+1]);
        else if(strcmp(argv[curr],"-B")==0)
            buffer_size=atoi(argv[curr+1]);
        else if(strcmp(argv[curr],"-p")==0)
            pg_prob=atof(argv[curr+1]);
        else if(strcmp(argv[curr],"-queue")==0)
            Queue=argv[curr+1];
        else if(strcmp(argv[curr],"-K")==0)
            K_KnockOut=atof(argv[curr+1]);
        else if(strcmp(argv[curr],"-out")==0)
            fileName=argv[curr+1];
        else if(strcmp(argv[curr],"-T")==0)
            max_timeslots=atol(argv[curr+1]);
        else {flag=0;break;}
            curr=curr+2;
    }

    srand(time(0)+19);
    //FSTREAM OBJECT TO OPEN A OUTPUT FILE TO WRITE THE OUTPUT OF ALGORITHMS TO IT.
    fout.open(fileName.c_str(),std::ios_base::app);
    if(fout != NULL)
    {
        fout << " N\t|\tp\t|\tQueue type\t|\tAvg PD\t|\tStd Dev of PD\t|\tAvg link utilization "<<endl;
        fout <<"------------------------------------------------------------------------------------------------------------------------"<<endl;
    }
    else
    {
        cout << "Some error has occurred.File can't be open!" <<endl;
        exit(1);
    }
    //CALL TO THE RESPECTIVE ALGORTIHM BASED ON THE QUEUE TYPE.
    transform(Queue.begin(), Queue.end(), Queue.begin(), ::tolower);
    if(Queue.compare("islip")==0)
        call_islip();
    if(Queue.compare("inq")==0)
        INQ(pg_prob,max_timeslots);
    if(Queue.compare("kouq")==0)
        KOUQ(pg_prob,max_timeslots,K_KnockOut);
    return 0;
}

