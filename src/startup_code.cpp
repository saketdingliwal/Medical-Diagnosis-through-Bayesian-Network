#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include<bits/stdc++.h>

// Format checker just assumes you have Alarm.bif and Solved_Alarm.bif (your file) in current directory
using namespace std;



// Our graph consists of a list of nodes where each node is represented as follows:
class Graph_Node{
public:
	string Node_Name;  // Variable name
	vector<int> Children; // Children of a particular node - these are index of nodes in graph.
	vector<int> Parents; // Parents of a particular node- note these are names of parents
	int nvalues;  // Number of categories a variable represented by this node can take
	vector<string> values; // Categories of possible values
	vector<double> CPT; // conditional probability table as a 1-d array . Look for BIF format to understand its meaning
	vector<double> updated_CPT;
	vector <double> final_CPT;

	// Constructor- a node is initialised with its name and its categories
  Graph_Node(string name,int n,vector<string> vals)
	{
		Node_Name=name;
		nvalues=n;
		values=vals;
	}
	string get_name()
	{
		return Node_Name;
	}
	vector<int> get_children()
	{
		return Children;
	}
	vector<int> get_Parents()
	{
		return Parents;
	}
	vector<double> get_CPT()
	{
		return CPT;
	}
	int get_nvalues()
	{
		return nvalues;
	}
	vector<string> get_values()
	{
		return values;
	}
	void set_CPT(vector<double> new_CPT)
	{
		CPT.clear();
		CPT=new_CPT;
	}
  void set_Parents(vector<int> Parent_Nodes)
  {
    Parents.clear();
    Parents=Parent_Nodes;
  }
  // add another node in a graph as a child of this node
  int add_child(int new_child_index)
  {
    for(int i=0;i<Children.size();i++)
    {
      if(Children[i]==new_child_index)
          return 0;
    }
    Children.push_back(new_child_index);
    return 1;
  }
	int get_value_index(string val)
	{
		for(int i=0;i<nvalues;i++)
		{
			if(values[i].compare(val)==0)
			{
				return i;
			}
		}
		return -1;
	}
};


 // The whole network represted as a list of nodes
class network{
public:
	vector<Graph_Node> Pres_Graph;
	int addNode(Graph_Node node)
	{
		Pres_Graph.push_back(node);
		return 0;
	}
	int netSize()
	{
		return Pres_Graph.size();
	}
  // get the index of node with a given name
  int get_index(string val_name)
  {
    int count=0;
    for(int i=0;i<Pres_Graph.size();i++)
    {
      if(Pres_Graph[i].get_name().compare(val_name)==0)
        return count;
      count++;
    }
    return -1;
  }
};
network Alarm;
void read_network()
{
	string line;
	int find=0;
	ifstream myfile("../dataset/alarm.bif");
	string temp;
	string name;
	vector<string> values;
  if (myfile.is_open())
  {
  	while (! myfile.eof() )
  	{
  		stringstream ss;
    	getline (myfile,line);
    	ss.str(line);
   		ss>>temp;
   		if(temp.compare("variable")==0)
   		{
 				ss>>name;
 				getline (myfile,line);
 				stringstream ss2;
 				ss2.str(line);
 				for(int i=0;i<4;i++)
 				{
 					ss2>>temp;
 				}
 				values.clear();
 				while(temp.compare("};")!=0)
 				{
 					values.push_back(temp);
 					ss2>>temp;
				}
 				Graph_Node new_node(name,values.size(),values);
 				int pos=Alarm.addNode(new_node);
   		}
   		else if(temp.compare("probability")==0)
   		{
 				ss>>temp;
 				ss>>temp;
        int index;
        int parent_index;
 				index=Alarm.get_index(temp);
        ss>>temp;
        values.clear();
 				while(temp.compare(")")!=0)
 				{
          parent_index=Alarm.get_index(temp);
					Alarm.Pres_Graph[parent_index].add_child(index);
 					Alarm.Pres_Graph[index].Parents.push_back(parent_index);
 					ss>>temp;
				}
				getline (myfile,line);
 				stringstream ss2;
 				ss2.str(line);
 				ss2>> temp;
 				ss2>> temp;
 				while(temp.compare(";")!=0)
 				{
					Alarm.Pres_Graph[index].CPT.push_back(0.001); // TODO assuming only -1 to be in input
					Alarm.Pres_Graph[index].updated_CPT.push_back(0.001);
					Alarm.Pres_Graph[index].final_CPT.push_back(0.0);

 					ss2>>temp;
				}
   		}
      else
      {
      }
  	}
  	if(find==1)
  	myfile.close();
	}
}
class sample
{
public:
	vector <int> values_points;
	int missing_index;
	vector <double> missing_value;
};


vector<sample> sample_list;

void read_samples()
{
	int find=0;
	ifstream myfile("../dataset/records.dat");
	if (myfile.is_open())
  {
  	while (! myfile.eof() )
  	{
			sample this_sample;
			this_sample.missing_index = -1;
			string record;
			getline (myfile,record);
			istringstream buf(record);
	    istream_iterator<std::string> beg(buf), end;
	    vector<string> tokens(beg, end);
			if(tokens.size()!=Alarm.Pres_Graph.size())
				continue;
			for(int i=0;i<tokens.size();i++)
			{
				string value = tokens[i];
				int value_index = Alarm.Pres_Graph[i].get_value_index(value);
				if(value_index==-1)
				{
					this_sample.missing_index = i;
					this_sample.values_points.push_back(0);
					for(int j=0;j<Alarm.Pres_Graph[i].nvalues;j++)
					{
						this_sample.missing_value.push_back((1.0/Alarm.Pres_Graph[i].nvalues));
					}
				}
				else
					this_sample.values_points.push_back(value_index);
			}
			sample_list.push_back(this_sample);
		}
		if(find==1)
  	myfile.close();
	}
}

int get_cpt_index(int node_index,int sample_index,int node_value)
{
	int cpt_index = 0;
	if(node_value==-1)
		node_value = sample_list[sample_index].values_points[node_index];
	int num_parents = Alarm.Pres_Graph[node_index].Parents.size();
	int prod = 1;
	for(int i=num_parents-1;i>=0;i--)
	{
		int parent_index = Alarm.Pres_Graph[node_index].Parents[i];
		int parent_value = sample_list[sample_index].values_points[parent_index];
		cpt_index += (parent_value*prod);
		prod = prod * (Alarm.Pres_Graph[parent_index].values.size());
	}
	cpt_index += (node_value*prod);
	return cpt_index;
}
void update(int node_index,int sample_index,int direction,double weight)
{
	int cpt_index = get_cpt_index(node_index,sample_index,-1);
	Alarm.Pres_Graph[node_index].updated_CPT[cpt_index] += (weight * direction);
}

void initialize_distribution()
{
	for(int i=0;i<sample_list.size();i++)
	{
		for(int j=0;j<Alarm.Pres_Graph.size();j++)
		{
			int miss = sample_list[i].missing_index;
			if(miss==-1)
			{
				update(j,i,1,1.0);
			}
			else
			{
				for(int k=0;k<Alarm.Pres_Graph[miss].nvalues;k++)
				{
					sample_list[i].values_points[miss] = k;
					double update_amt = sample_list[i].missing_value[k];
					update(j,i,1,update_amt);
				}
			}
		}
	}
}

void debug_print()
{
	for(int i=0;i<Alarm.Pres_Graph.size();i++)
	{
		cout << Alarm.Pres_Graph[i].Node_Name <<endl;
		for(int j=0;j<Alarm.Pres_Graph[i].updated_CPT.size();j++)
		{
			cout << Alarm.Pres_Graph[i].final_CPT[j] << endl;
		}
		cout << "-----------------------------------------------"<<endl;
	}
}

void update_probab(int sample_index,int direction)
{
	int miss = sample_list[sample_index].missing_index;
	for(int i=0;i<Alarm.Pres_Graph[miss].nvalues;i++)
	{
		double update_amt = sample_list[sample_index].missing_value[i];
		sample_list[sample_index].values_points[miss] = i;
		update(miss,sample_index,direction,update_amt);
	}
	for(int i=0;i<Alarm.Pres_Graph[miss].Children.size();i++)
	{
		for(int j=0;j<Alarm.Pres_Graph[miss].nvalues;j++)
		{
			sample_list[sample_index].values_points[miss] = j;
			double update_amt = sample_list[sample_index].missing_value[j];
			update(Alarm.Pres_Graph[miss].Children[i],sample_index,direction,update_amt);
		}

	}
}

double probab(int node_index,int sample_index)
{
	int cpt_index = get_cpt_index(node_index,sample_index,-1);
	double numerator = Alarm.Pres_Graph[node_index].CPT[cpt_index];
	double denominator = 0;
	for(int i=0;i<Alarm.Pres_Graph[node_index].nvalues;i++)
	{
		int cpt_index = get_cpt_index(node_index,sample_index,i);
		denominator += Alarm.Pres_Graph[node_index].CPT[cpt_index];
	}
	double probab = (1.0 * numerator) / denominator;
	return probab;
}

void update_sample(int sample_index)
{
	int miss = sample_list[sample_index].missing_index;
	double probabs[Alarm.Pres_Graph[miss].nvalues];
	int poss_values = Alarm.Pres_Graph[miss].nvalues;
	double comm_probabs[Alarm.Pres_Graph[miss].nvalues+1];
	comm_probabs[0] = 0;
	for(int i=0;i<Alarm.Pres_Graph[miss].nvalues;i++)
	{
		sample_list[sample_index].values_points[miss] = i;
		probabs[i] = probab(miss,sample_index);
		for(int j=0;j<Alarm.Pres_Graph[miss].Children.size();j++)
		{
			probabs[i] *= (probab(Alarm.Pres_Graph[miss].Children[j],sample_index));
		}
		comm_probabs[i+1] = comm_probabs[i] + probabs[i];
	}
	for(int i=0;i<Alarm.Pres_Graph[miss].nvalues;i++)
	{
		sample_list[sample_index].missing_value[i] = (1.0 * probabs[i])/(comm_probabs[Alarm.Pres_Graph[miss].nvalues]);
	}
}

void print_network()
{
	string line;
	int find=0;
	ifstream myfile("../dataset/alarm.bif");
	string temp;
	string name;
	vector<string> values;
  if (myfile.is_open())
  {
  	while (! myfile.eof() )
  	{
  		stringstream ss;
    	getline (myfile,line);
			cout << line<<endl;
    	ss.str(line);
   		ss>>temp;
   		if(temp.compare("variable")==0)
   		{
 				ss>>name;
 				getline (myfile,line);
				cout << line<<endl;
   		}
   		else if(temp.compare("probability")==0)
   		{
 				ss>>temp;
 				ss>>temp;
        int index;
 				index=Alarm.get_index(temp);
				getline (myfile,line);
 				stringstream ss2;
 				ss2.str(line);
 				ss2>> temp;
				cout << temp << " ";
 				ss2>> temp;
				int j=0;
 				while(temp.compare(";")!=0)
 				{
					cout << Alarm.Pres_Graph[index].final_CPT[j]<<" ";
					j++;
 					ss2>>temp;
				}
				cout << temp << endl;
   		}
      else
      {
      }
  	}
  	if(find==1)
  	myfile.close();
	}
}


int main(int argc,char * argv[])
{
	time_t start = static_cast <unsigned> (time(0));
	read_network();
	read_samples();
	initialize_distribution();
	// debug_print();
	int max_iter = atoi(argv[1]);
	for(int i=0;i<max_iter;i++)
	{
		time_t end = static_cast <unsigned> (time(0));
		// cout << difftime(end,start);
		// if(i%100==0)
		// 	// cout << i << endl;

		// if(difftime(end,start)>4)
		// {
		// 	break;
		// }
		for(int j=0;j<Alarm.Pres_Graph.size();j++)
		{
			for(int k=0;k<Alarm.Pres_Graph[j].CPT.size();k++)
				 Alarm.Pres_Graph[j].CPT[k] = Alarm.Pres_Graph[j].updated_CPT[k];
		}
		for(int j=0;j<sample_list.size();j++)
		{
			if(sample_list[j].missing_index==-1)
				continue;
			update_probab(j,-1);
			update_sample(j);
			update_probab(j,1);
		}
	}

	for(int i=0;i<Alarm.Pres_Graph.size();i++)
	{
		int cpt_len = Alarm.Pres_Graph[i].CPT.size();
		int nvals = Alarm.Pres_Graph[i].nvalues;
		int imp = cpt_len/nvals;
		double summs[cpt_len/nvals];
		for(int j=0;j<imp;j++)
		{
			summs[j] = 0.0;
		}
		for(int j=0;j<cpt_len;j++)
		{
			summs[j%imp] += Alarm.Pres_Graph[i].CPT[j];
		}
		for(int j=0;j<cpt_len;j++)
		{
			Alarm.Pres_Graph[i].final_CPT[j] =  (1.0 * Alarm.Pres_Graph[i].CPT[j]) / summs[j%imp] ;
		}
	}
	// debug_print();
	print_network();

}
