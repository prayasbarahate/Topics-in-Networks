// TIN1.cpp : Defines the entry point for the console application.
#include <iostream>
#include <fstream>
#include <limits.h>
#include<time.h>
#include <bits/stdc++.h> 

using namespace std;
//FILESTRAM INPUT OUTPIT OBJECTS
ifstream fin;
ofstream fout,fout1,fout2;

//GLOBAL VARIABLES
int NodeCount, EdgeCount,Con_request , req_accepted;
int const ArrSize = 1000;
int Topology[ArrSize][ArrSize];
int Delay[ArrSize][ArrSize];
float Capacity[ArrSize][ArrSize];
int pathS1[ArrSize][ArrSize];
int pathS2[ArrSize][ArrSize];
int RoutingTable1[ArrSize][ArrSize];
int RoutingTable2[ArrSize][ArrSize];
int Granted_Connections[ArrSize][ArrSize];
int count_of_source;
int global_VCID = 1000;

//FUNCTION FOR INITIALIZATION.
void initialize()
{
	for (int i = 0; i < NodeCount; i++)
	{
		for (int j = 0; j < NodeCount; j++)
		{
			Topology[i][j] = INT_MAX;
			Delay[i][j] = INT_MAX;
		}
	}
	for (int i = 0; i < NodeCount*NodeCount; i++)
	{
		for (int j = 0; j < NodeCount+2; j++)
		{
			RoutingTable1[i][j] = INT_MAX;
			RoutingTable2[i][j] = INT_MAX;
		}
	}
	for (int i = 0; i < NodeCount; i++)
	{
		for (int t = 0; t < NodeCount; t++)
		{
			pathS1[i][t] = t;
			pathS2[i][t] = t;
		}
	}
}

//fOR READING FILE INPUTS FROM FILES
void Read_Values(string filename1)
{
	fin.open(filename1);
	fin >> NodeCount >> EdgeCount;
	initialize();
	int source = 0;
	if (fin.is_open())
	{
		while (fin >> source)
		{
			int dest = 0, delay = 0;
			float cap = 0.0;
			float extra = 0;
			fin >> dest;
			fin >> delay;
			fin >> cap;
			fin >> extra;
			Topology[source][dest] = 1;
			Topology[dest][source] = 1;
			Delay[source][dest] = delay;
			Delay[dest][source] = delay;
			Capacity[source][dest] = cap;
			Capacity[dest][source] = cap;
		}
	}
	else
	{
		cout << "file can't be opened.";
		exit(-1);
	}
	fin.close();
}

//DIJKSTRA'S ALGO TO FIND SHORTEST PATH FOR ALL SOURCE DESTINATION PAIR.
void shortd_first()
{
	count_of_source =0;
	int dist[NodeCount][NodeCount];
	for (int i = 0; i < NodeCount; i++)
		for (int j = 0; j < NodeCount; j++)
			dist[i][j] = Topology[i][j];
	int distance[ArrSize][ArrSize];
	for (int startnode = 0; startnode < NodeCount; startnode++)
	{
		int visited[NodeCount], count, mindistance, nextnode, i, j;

		for (i = 0; i < NodeCount; i++)
		{
			distance[startnode][i] = dist[startnode][i];
			pathS1[startnode][i] = startnode;
			visited[i] = 0;
		}
		distance[startnode][startnode] = 0;
		visited[startnode] = 1;
		count = 1;
		while (count < NodeCount - 1)
		{
			mindistance = INT_MAX;
			for (i = 0; i < NodeCount; i++)
			{
				if (distance[startnode][i] < mindistance && !visited[i])
				{
					mindistance = distance[startnode][i];
					nextnode = i;
				}
			}
			visited[nextnode] = 1;
			for (i = 0; i < NodeCount; i++)
			{
				if (!visited[i])
				{
					if (dist[nextnode][i] != INT_MAX && mindistance + dist[nextnode][i] < distance[startnode][i])
					{
						distance[startnode][i] = mindistance + dist[nextnode][i];
						pathS1[startnode][i] = nextnode;
					}
				}
			}
			count++;
		}
	}
	//FOR STORING THE PATH IN THE MATRIX
	for (int v = 0; v < NodeCount; v++)
	{
		for (int i = 0; i < NodeCount; i++)
		{
			if (i != v)
			{
				RoutingTable1[count_of_source][0]=v;
				RoutingTable1[count_of_source][1]=i;
				int col=2;
				vector<int> path;
				path.push_back(i);
				int j = i;
				do
				{
					j = pathS1[v][j];
					path.push_back(j);
				} while (j != v);
				reverse(path.begin(),path.end());	
				for(int k =0; k < path.size();k++)
					RoutingTable1[count_of_source][col++] = path[k];
				RoutingTable1[count_of_source][col] = col-3;
				count_of_source++;
			}
		}
	}
}
//FUNCTION TO FIND THE SECOND SHORTEST PATH WHICH IS LINK DISJOINT ALSO FOR EACH SOURCE DESTINATION PAIR.
void shortd_second(int startnode , int destnode)
{
	int dist[NodeCount][NodeCount];
	for (int i = 0; i < NodeCount; i++)
		for (int j = 0; j < NodeCount; j++)
			dist[i][j] = Topology[i][j];

	int j = destnode;
	do
	{
		int next = pathS1[startnode][j];
		dist[j][next] = INT_MAX;
		dist[next][j] = INT_MAX;
		j = next;
	} while (j != startnode);

	int distance[ArrSize][ArrSize];
	for (int startnode = 0; startnode < NodeCount; startnode++)
	{
		int visited[NodeCount], count, mindistance, nextnode, i, j;

		for (i = 0; i < NodeCount; i++)
		{
			distance[startnode][i] = dist[startnode][i];
			pathS2[startnode][i] = startnode;
			visited[i] = 0;
		}
		distance[startnode][startnode] = 0;
		visited[startnode] = 1;
		count = 1;
		while (count < NodeCount - 1)
		{
			mindistance = INT_MAX;
			for (i = 0; i < NodeCount; i++)
			{
				if (distance[startnode][i] < mindistance && !visited[i])
				{
					mindistance = distance[startnode][i];
					nextnode = i;
				}
			}
			visited[nextnode] = 1;
			for (i = 0; i < NodeCount; i++)
			{
				if (!visited[i])
				{
					if (dist[nextnode][i] != INT_MAX && mindistance + dist[nextnode][i] < distance[startnode][i])
					{
						distance[startnode][i] = mindistance + dist[nextnode][i];
						pathS2[startnode][i] = nextnode;
					}
				}
			}
			count++;
		}
	}
	//FOR STORING THE PATH IN THE MATRIX
	int v = startnode;
	int i = destnode;
	if (i != v)
	{
		RoutingTable2[count_of_source][0] = v;
		RoutingTable2[count_of_source][1] = i;
		int col = 2;
		vector<int> path;
		path.push_back(i);
		int j = i;
		do
		{
			j = pathS2[v][j];
			path.push_back(j);
		} while (j != v);
		reverse(path.begin(), path.end());

		for (int k = 0; k < path.size(); k++)
		{
			RoutingTable2[count_of_source][col] = path[k];
			col++;
		}
		RoutingTable2[count_of_source][col] = col - 3;
		count_of_source++;
	}
}

//fUNCTION TO CHECK FOR THE REQUESTS TO BE ACCEPTED OR REJECTED.
void Conn_Request(string filename, string forfile , string connectionfile, int is_optimistic)
{
	fin.open(filename); //Read codebook file into matrix
	fout.open(forfile,std::ofstream::out | std::ofstream::trunc);
	fout1.open(connectionfile,std::ofstream::out | std::ofstream::trunc);
	fin >> Con_request;
	int startn = 0;
	if (fin.is_open())
	{
		fout << "Router's ID\tIncoming Port ID\t\tVCID\t\tOutgoing Port ID\t\tVCID\n";
		fout1 << "Connection ID\tSource\t\tDestination\t\tPath\t\t\tVCID List\t\t\tPath Cost\n";
		int request_no = 0;
		while (fin >> startn)
		{
			int destn = 0, bmin = 0, bav = 0, bmax = 0;
			float b_equiv = 0;
			fin >> destn;
			fin >> bmin;
			fin >> bav;
			fin >> bmax;

			if (is_optimistic == 0)
			{
				b_equiv = bmax < (bav + 0.25 * (bmax - bmin)) ? bmax : (bav + 0.25 * (bmax - bmin));
			}
			else
			{
				b_equiv = bmax;
			}

			int i = 0, k = 2, col1 = 2, flag = 0;
			while (RoutingTable1[i][0] != startn || RoutingTable1[i][1] != destn)
				i++;
			while (RoutingTable1[i][k] != INT_MAX)
			{
				k++;
			}
			while (col1 < k - 2)
			{
				int s = RoutingTable1[i][col1++];
				int t = RoutingTable1[i][col1];
				if (b_equiv > Capacity[s][t])
				{
					cout << "request failed for request = " << request_no <<" from source = " << startn << " to destination = " << destn << " for first shortest path." << endl;
					flag = 1;
					break;
				}
			}
			int col = 2;
			int prev = INT_MAX;
			int next = INT_MAX;
			if (flag == 0)
			{
				while (col < k - 1)
				{
					//WRITING TO FORWARDING TABLE FILE.
					int s = RoutingTable1[i][col++];
					int t = RoutingTable1[i][col];
					if (col < k - 2)
					{
						Capacity[s][t] -= b_equiv;
					}
					if (s == destn)
						next = INT_MAX;
					else
						next = t;
					fout << s << "\t\t\t";
					if (prev == INT_MAX)
						fout << "---"
							 << "\t\t\t";
					else
						fout << prev << "\t\t\t";
					fout << global_VCID << "\t\t\t";
					if (next == INT_MAX)
						fout << "---"
							 << "\t\t\t";
					else
						fout << next << "\t\t\t";
					fout << global_VCID << endl;
					prev = s;
				}
				//Writing to connection table file.
				fout1 << request_no << "\t\t";
				fout1 << startn << "\t\t" << destn << "\t\t\t";
				int col3 = 2 , r;
				for(r = col3 ; RoutingTable1[i][r] != destn ; r++)
				{
					fout1 << RoutingTable1[i][r] << "->";
				}
				fout1 << destn << "\t\t\t";
				int cost = RoutingTable1[i][r+1];
				int factor = rand() % 100 ;
				int gvc = global_VCID;
				for(int s = 0 ; s < cost ; s++)
				{
					fout1 << gvc - (s*factor);
					if(s<cost-1)
						fout1 << ",";
				}
				fout1 << "\t\t\t\t";
				fout1 <<cost << endl; 

				req_accepted++;
				global_VCID += 200;
			}
			else if(flag == 1)
			{
				if (is_optimistic == 0)
				{
					b_equiv = bmax < (bav + 0.25 * (bmax - bmin)) ? bmax : (bav + 0.25 * (bmax - bmin));
				}
				else
				{
					b_equiv = bmax;
				}

				int i = 0, k = 2, col1 = 2, flag = 0;
				while (RoutingTable2[i][0] != startn || RoutingTable2[i][1] != destn)
					i++;
				while (RoutingTable2[i][k] != INT_MAX)
				{
					k++;
				}
				while (col1 < k - 2)
				{
					int s = RoutingTable2[i][col1++];
					int t = RoutingTable2[i][col1];
					if (b_equiv > Capacity[s][t])
					{
						cout << "request failed for request = " << request_no << " from source = " << startn << " to destination = " << destn << " second shortest path also." << endl;
						flag = 2;
						break;
					}
				}
				int col = 2;
				int prev = INT_MAX;
				int next = INT_MAX;
				if (flag == 1)
				{
					while (col < k - 1)
					{
						int s = RoutingTable2[i][col++];
						int t = RoutingTable2[i][col];
						if (col < k - 2)
						{
							Capacity[s][t] -= b_equiv;
						}
						if (s == destn)
							next = INT_MAX;
						else
							next = t;
						fout << s << "\t\t\t";
						if (prev == INT_MAX)
							fout << "---"
								 << "\t\t\t";
						else
							fout << prev << "\t\t\t";
						fout << global_VCID << "\t\t\t";
						if (next == INT_MAX)
							fout << "---"
								 << "\t\t\t";
						else
							fout << next << "\t\t\t";
						fout << global_VCID << endl;
						prev = s;
					}
					//Writing to connection table file.
					fout1 << request_no << "\t\t";
					fout1 << startn << "\t\t" << destn << "\t\t\t";
					int col3 = 2, r;
					for (r = col3; RoutingTable2[i][r] != destn; r++)
					{
						fout1 << RoutingTable2[i][r] << "->";
					}
					fout1 << destn << "\t\t\t";
					int cost = RoutingTable2[i][r + 1];
					int factor = rand() % 100;
					int gvc = global_VCID;
					for (int s = 0; s < cost; s++)
					{
						fout1 << gvc - (s * factor);
						if (s < cost - 1)
							fout1 << ",";
					}
					fout1 << "\t\t\t\t";
					fout1 << cost << endl;

					req_accepted++;
					global_VCID += 200;
				}
			}
			request_no++;
		}
	}
	else
	{
		cout << "file can't be opened.";
		exit(-1);
	}
	fin.close();
	fout.close();
	fout1.close();
}

int main(int argc, char *argv[])
{
	//READING COMMAND LINE ARGUMENTS
	if (argc < 8)
    {
        printf("No or less arguments are provided.\n");
        exit(1);
    }
	string topfile = argv[1];
	string confile = argv[2];
	string routfile = argv[3];
	string forfile = argv[4];
	string pathfile = argv[5];	
    int hop_dis = atoi(argv[6]);
    int is_opti = atoi(argv[7]);

	Read_Values(topfile);
	srand(time(0));
	
	//IF DELAY OR DISTANCE IS THE MATRIC FOR CALCULATING THE SHORTEST PATH.
	if (hop_dis == 1)
	{
		for (int i = 0; i < NodeCount; i++)
		{
			for (int j = 0; j < NodeCount; j++)
			{
				Topology[i][j] = Delay[i][j];
			}
		}
	}

	//APPLYING FLOYD WARSHALL'S ALGORITHM TO FIND THE ALL PAIR SHORTEST PATH.
	shortd_first();
	count_of_source = 0;
	for (int i = 0; i < NodeCount; i++)
	{
		for (int j = 0; j < NodeCount; j++)
			shortd_second(i, j);
	}
	//WRITING TO ROUTING TABLE FILE.
	fout2.open(routfile,std::ofstream::out);
	fout2 << "Source Node\tDestination Node\tPath\t\t\t\t\tPath Delay\t\t\tPath Cost\n";
	for (int i = 0; i < count_of_source; i++)
	{
		int col =0 , r, delay = 0,destn;
		while (RoutingTable1[i][col] != INT_MAX)
			col++;
		if (col > 0)
		{
			fout2 << RoutingTable1[i][0]<<"\t\t\t";
			destn = RoutingTable1[i][1];
			fout2 << destn << "\t\t";
			int col3 = 2, r;
			for (r = col3; RoutingTable1[i][r] != destn; r++)
			{
				fout2 << RoutingTable1[i][r] << "->";
			}
			fout2 << destn << "\t\t\t";
			col3 = 2;
			while (col3 < col - 2)
			{
				int s = RoutingTable1[i][col3++];
				int t = RoutingTable1[i][col3];
				delay += Delay[s][t];
			}
			fout2 << "\t\t\t";
			fout2 << delay << "\t\t\t";
			fout2 << RoutingTable1[i][r + 1] << endl;
		}

		int col1 =0 ,delay1 = 0,destn1;
		while (RoutingTable2[i][col1] != INT_MAX)
			col1++;
		if (col1 > 0)
		{
			fout2 << RoutingTable1[i][0]<<"\t\t\t";
			destn1 = RoutingTable2[i][1];
			fout2 << destn1 << "\t\t";
			int col3 = 2, r;
			for (r = col3; RoutingTable2[i][r] != destn1; r++)
			{
				fout2 << RoutingTable2[i][r] << "->";
			}
			fout2 << destn1 << "\t\t\t";
			col3 = 2;
			while (col3 < col1 - 2)
			{
				int s = RoutingTable2[i][col3++];
				int t = RoutingTable2[i][col3];
				delay1 += Delay[s][t];
			}
			fout2 << "\t\t\t";
			fout2 << delay1 << "\t\t\t";
			fout2 << RoutingTable2[i][r + 1] << endl;
		}
	}

	fout2.close();
	string Connection_file = "ConnectionTable.txt";
	Conn_Request(confile,forfile,Connection_file,is_opti);
	//DUMPING DATA IN PATH FILE.
	cout << "Requests Accepted " << req_accepted << endl;
	fout.open(pathfile,std::ofstream::out);
	fout << "Total Connection Requested are : " << Con_request << endl;
	fout << "Toatl Accepted Requests are : " << req_accepted << endl;
	fout.close();
	return 0;
}
//END OF FILE.