#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>


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
	vector<int> CPT; // conditional probability table as a 1-d array . Look for BIF format to understand its meaning
	vector<int> updated_CPT;


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
	vector<int> get_CPT()
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
	void set_CPT(vector<int> new_CPT)
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

network read_network()
{
	network Alarm;
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
					Alarm.Pres_Graph[index].CPT.push_back(0); // TODO assuming only -1 to be in input
					Alarm.Pres_Graph[index].updated_CPT.push_back(0);
 					// 	curr_CPT.push_back(atof(temp.c_str()));
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
	return Alarm;
}
int main()
{
	network Alarm;
	Alarm=read_network();
}
