#include <string>
#include <iostream>
#include <fstream>
#include <boost/unordered_map.hpp>
#include "graph.hpp"

using std::string;
using std::ifstream;
using std::cout;
using boost::unordered_map;

typedef unordered_map<string,string> idtoval;

/* returns string with next tag */
string nextTag(ifstream *fin)
{
  string result = "";
  char c = (*fin).get();
  while(c!='<')
  {
    if((*fin).good())
      c=(*fin).get();
    else
      return "**EOF**";
  }
  while(c!='>')
  {
    c = (*fin).get();
    result += c;
  }
  result=result.substr(0,result.size()-1);
  return result;
}

/* extracts attribute from string */
string extract(string s, string attribute)
{
  attribute = " "+attribute+"=";
  std::size_t loc = s.find(attribute);
  if(loc != std::string::npos)
  {
    s = s.substr(loc);
    s = s.substr(s.find("\"")+1);
    s = s.substr(0,s.find("\""));
    return s;
  }
  return "**EOS**";
}

/* extracts the title of the gexf */
string getAttribute(ifstream *fin)
{
  string cur = nextTag(fin);
  while(cur.substr(0,10) != "attributes")
  {
    cur = nextTag(fin);
    if(cur == "**EOF**") {cout << "read attribute error"; break;}
  }
  while(cur != "/attributes")
  {
    cur = nextTag(fin);
    if(cur == "**EOF**") {cout << "read attribute error"; break;}
    string title = extract(cur,"title");
    if(title != "**EOS**")
      return title;
  }
  return "";
}

/* puts the nodes into target */
void getNodes(ifstream *fin, Graph *target, idtoval *itv)
{
  string cur = nextTag(fin);
  while(cur.substr(0,5) != "nodes")
  {
    cur = nextTag(fin);
    if(cur == "**EOF**") {cout << "read nodes error"; break;}
  }
  string curid;
  while(cur != "/nodes")
  {
    cur = nextTag(fin);
    if(cur == "**EOF**") {cout << "read nodes error"; break;}
    string id = extract(cur,"id");
    if(id != "**EOS**")
      curid = id;
    string value = extract(cur,"value");
    if(value != "**EOS**")
    {
      (*target).addVertex(value);
      (*itv)[curid] = value;
    }
  }
}

/* puts the edges into target */
void getEdges(ifstream *fin, Graph *target, idtoval *itv)
{
  string cur = nextTag(fin);
  while(cur.substr(0,5) != "edges")
  {
    cur = nextTag(fin);
    if(cur == "**EOF**") {cout << "read edges error"; break;}
  }
  while(cur != "/edges")
  {
    cur = nextTag(fin);
    if(cur == "**EOF**") {cout << "read edges error"; break;}
    string id1 = extract(cur, "source");
    string id2 = extract(cur, "target");
    if(id1 != "**EOS**" && id2 != "**EOS**")
      (*target).addEdge((*itv)[id1], (*itv)[id2]);
  }
}

/* returns the adjacency list stored in fileName */
Graph readFromGexf(string fileName)
{
  Graph result;
  idtoval itv;
  ifstream fin(fileName);
  unsigned start = fileName.find_last_of("/");
  unsigned end = fileName.find(".gexf");
  if(start == string::npos) start = -1;
  string name = fileName.substr(start+1,end-start-1);
  result.setName(name);
  string atr = getAttribute(&fin);
  if(atr == "gname")
  {
    getNodes(&fin, &result, &itv);
    getEdges(&fin, &result, &itv);
  }
  else
  {
    cout << "gexf does not have attribute gname\n";
    exit(0);
  }
  return result;
}

