/*
 * Copyright (C) 2022  Ferdous,S M <ferdous.csebuet@egmail.com>
 * Author: Ferdous,S M <ferdous.csebuet@egmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "GraST/Utility.h"
#include <fstream>
#include <iostream>
#include <string>
#include <random>
#include <algorithm>
#include <cmath>

void permuteInput(std::string infile, std::string outfile) {
  
  std::ifstream fileRead(infile.c_str());
  if(fileRead.is_open()==false)
  {
    std::cout << "No file named "<<infile<<std::endl;
    std::exit(1);
  }
  std::ofstream fileWrite(outfile.c_str());

  if(fileWrite.is_open()==false)
  {
    std::cout << "Could not open file named "<<outfile<<std::endl;
    std::exit(1);
  }
  //Read the mtx header to determine file type 
  std::string header[6];
  fileRead>>header[0]>>header[1]>>header[2]>>header[3]>>header[4];

  std::string weightType = header[3];
  fileRead.seekg(0, std::ios::beg); 
  
  std::string desc;
  while (fileRead.peek() == '%') {
    std::getline(fileRead,desc); 
    fileWrite << desc<<std::endl;
  
  } 
  NODE_T nrow,ncol;
  EDGE_T nnz;
  fileRead >> nrow >> ncol >> nnz;
  fileWrite << nrow <<" "<< ncol <<" "<< nnz<<std::endl;

  WeightEdgeSimList edgeList;
  edgeList.reserve(nnz);

  NODE_T u;
  NODE_T v;
  VAL_T weight;
  EDGE_T nEdge = 0;


  for(EDGE_T i=0;i<nnz;i++)
  {
    weight = 1.0;
    if(weightType.compare("pattern") != 0)
      fileRead >> v >> u >> weight;
    else
      fileRead >> v >> u;


      edgeList.push_back({u,v,weight});    
      nEdge++;
    //std::cout<<u<<" "<<v<<" "<<weight<<std::endl;
  }
  std::random_device rd;
  std::mt19937 g(rd());

  std::shuffle(edgeList.begin(), edgeList.end(), g);

  for(auto e:edgeList)
  {
    if(weightType.compare("pattern") != 0)
      fileWrite << e.v << " "<<e.u <<" "<< std::fixed<<e.weight<<std::endl;
    else
      fileWrite << e.v << " "<<e.u<<std::endl;
  }
  fileRead.close();
  fileWrite.close();
}

void readEdgeList(std::string fileName, NODE_T &n, EDGE_T &m, WeightEdgeSimList &edgeList) {
  
  std::ifstream fileRead(fileName.c_str());
  if(fileRead.is_open()==false)
  {
    std::cout << "No file named "<<fileName<<std::endl;
    std::exit(1);
  }

  //Read the mtx header to determine file type 
  std::string header[6];
  fileRead>>header[0]>>header[1]>>header[2]>>header[3]>>header[4];

  std::string weightType = header[3];
  fileRead.seekg(0, std::ios::beg); 

  //Ignore header and comments
  while (fileRead.peek() == '%') fileRead.ignore(2048, '\n');

  NODE_T nrow,ncol;
  EDGE_T nnz;
  fileRead >> nrow >> ncol >> nnz;

  edgeList.reserve(nnz);

  NODE_T u;
  NODE_T v;
  VAL_T weight;
  EDGE_T nEdge = 0;
  VAL_T minW = 1.0;
  VAL_T maxW = 1000000.0;
  bool is_random = false;
  std::mt19937 engine;
  engine.seed(341287);
  std::uniform_real_distribution<VAL_T> dist(minW,maxW);

  if(weightType.compare("pattern") == 0)
    is_random = 1;

  for(EDGE_T i=0;i<nnz;i++)
  {
    weight = 1.0;
    if(weightType.compare("pattern") != 0)
      fileRead >> v >> u >> weight;
    else
      fileRead >> v >> u;
    
    if(is_random == true)
    {
      //weight = minW + ((maxW-minW)*((VAL_T)rand()/RAND_MAX)); 
      weight = dist(engine);
    }

    v--;
    u--;

    if(weight < 0) weight = weight*(-1);

    if(u<v && weight > 0)
    {
      edgeList.push_back({u,v,weight});    
      nEdge++;
    }
    //std::cout<<u<<" "<<v<<" "<<weight<<std::endl;
  }
  fileRead.close();
  n = nrow;
  m = nEdge;
}

bool isValidMatching(NODE_T n, std::vector<WeightEdgeSim> &matchEdges)
{
  std::vector<bool> matched(n, false);

  for( auto e: matchEdges)
  {
    NODE_T u = e.u; 
    NODE_T v = e.v; 

    //std::cout<<u<<" "<<v<<" "<<e.weight<<std::endl;
    
    if(matched[u] != false && matched[v] == false)
      return 0;
    matched[u] = true;
    matched[v] = true;
  }
  return 1;
}

// Given an integer k as input, tests whether the matching is a valid k-matching
bool isValidkMatching(int k, NODE_T n, std::vector<WeightEdgeSim> &matchedEdges){
  // Numbers of mathched edges incident to each node
  std::vector<int> saturation(n, 0);

  for(WeightEdgeSim e: matchedEdges){
    NODE_T u = e.u; 
    NODE_T v = e.v; 

    saturation[u]++;
    saturation[v]++;
    // If any nodes becomes oversaturated, return false
    if (saturation[u] > k || saturation[v] > k)
      return false;
  }
  return true;
}

void writeOMtx(NODE_T n, AdjacencyList &adjList, std::string outFile) {

  std::ofstream outfile;
  outfile.open(outFile);
  outfile << n<<"\n";
  for(NODE_T u=0;u<n;u++) {
    if(adjList[u].empty() == false) {
      for(auto e:adjList[u]) {
        outfile<<u<<" "<<e.v<<" "<<e.weight<<"\n";
      }   
    }
  }
}

void readOMtx(NODE_T &n, AdjacencyList &adjList, std::string inFile) {

  std::ifstream infile;
  infile.open(inFile);
  infile >> n;
  std::cout<<n<<std::endl;
  adjList.resize(n);
  
  NODE_T u,v;
  VAL_T w;

  while(infile>>u>>v>>w) {
    adjList[u].push_back({v,w});
  }
  
}

/**
 * @brief Generates a GraphViz dot file from a list of edges.
 * 
 * @param edges The edge list that forms the graph
 * @param outFile The name of the output file
 */
void writeDotFile(std::vector<WeightEdgeSim> &edges, std::string outFile) {
  std::ofstream outfile;
  outfile.open(outFile);
  outfile << "strict graph {" << std::endl << "\tsplines=\"TRUE\";" << std::endl << std::endl;
  for (auto &e : edges) {
    outfile << "\t" << e.u << " -- " << e.v << std::endl;
  }
  outfile << "}";
}

/**
 * @brief Generates a GraphViz dot file from a list of edges and also colors them.
 * 
 * @param edges The edge list that forms the graph
 * @param mg The MisraGries object that contains the edge coloring
 * @param outFile The name of the output file
 * @param sat Controls the S value for the HSV color space
 * @param val Controls the V value for the HSV color space
 */
void writeDotFile(std::vector<WeightEdgeSim> &edges, MisraGries &mg, std::string outFile, const double sat, const double val) {
  // Generate color palette for edge colors
  // References: https://martin.ankerl.com/2009/12/09/how-to-create-random-colors-programmatically/
  double phi = (1 + sqrt(5))/2;
  std::unordered_map<COLOR_T, std::string> colorPalette;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0, 1); // Uniform distribution between 0 and 1
  double hue = dis(gen);
  double whole;
  for (int i = 0; i < mg.getNumColors(); i++) {
    hue += phi;
    hue = std::modf(hue, &whole);
    colorPalette[i] = std::to_string(hue).substr(0, 5) + " " + std::to_string(sat).substr(0, 5) + " " + std::to_string(val).substr(0, 5);
  }

  std::ofstream outfile;
  outfile.open(outFile);
  outfile << "strict graph {" << std::endl << 
    "\tsplines=\"TRUE\";" << std::endl << 
    //"\tlayout=\"circo\";" << std::endl << 
    "\tnode [shape=circle];" << std::endl <<
    "\tedge [penwidth=2];" << 
    std::endl << std::endl;
  for (auto &e : edges) {
    outfile << "\t" << e.u << " -- " << e.v << 
      " [color=\"" << colorPalette[mg.getEdgeColor(e)] << 
      "\", label=\"" << mg.getEdgeColor(e) << 
      "\"];" << std::endl;
  }
  outfile << "}";
}

/**
 * @brief Generates a GraphViz dot file from a list of edges and also colors them.
 * 
 * @param edges The edge list that forms the graph
 * @param mg The MisraGries object that contains the edge coloring
 * @param outFile The name of the output file
 * @param sat Controls the S value for the HSV color space
 * @param val Controls the V value for the HSV color space
 */
void writeDotFile(std::vector<std::vector<WeightEdgeSim>> &edges, std::string outFile, const double sat, const double val) {
  // Generate color palette for edge colors
  // References: https://martin.ankerl.com/2009/12/09/how-to-create-random-colors-programmatically/
  double phi = (1 + sqrt(5))/2;
  std::unordered_map<COLOR_T, std::string> colorPalette;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0, 1); // Uniform distribution between 0 and 1
  double hue = dis(gen);
  double whole;
  for (int i = 0; i < edges.size(); i++) {
    hue += phi;
    hue = std::modf(hue, &whole);
    colorPalette[i] = std::to_string(hue).substr(0, 5) + " " + std::to_string(sat).substr(0, 5) + " " + std::to_string(val).substr(0, 5);
  }

  std::ofstream outfile;
  outfile.open(outFile);
  outfile << "strict graph {" << std::endl << 
    "\tsplines=\"TRUE\";" << std::endl << 
    //"\tlayout=\"circo\";" << std::endl << 
    "\tnode [shape=circle];" << std::endl <<
    "\tedge [penwidth=2];" << 
    std::endl << std::endl;
  for (int i = 0; i < edges.size(); i++) {
    for (auto &e : edges[i]) {
      outfile << "\t" << e.u << " -- " << e.v << 
        " [color=\"" << colorPalette[i] << 
        "\", label=\"" << i << 
        "\"];" << std::endl;
    }
  }
  outfile << "}";
}
//generated by chatgpt on 7/20/23
std::string getLastPartOfFilepath(const std::string& filepath) {
    // Find the last occurrence of the path separator character (e.g., '/')
    size_t lastSeparator = filepath.find_last_of("/\\");

    if (lastSeparator != std::string::npos) {
        // Extract the substring starting from the position after the last separator
        return filepath.substr(lastSeparator + 1);
    }

    // If no separator is found, return the entire filepath
    return filepath;
}

/*
//inspired from https://www.youtube.com/watch?v=Os5cK0H8EOA
long long get_mem_usage() {
  struct rusage myusage;
  getrusage(RUSAGE_SELF, &myusage);
  return myusage.ru_maxrss;
}
*/

//generated by chatgpt 8/15/23
double standardDeviation(const std::vector<NODE_T>& values) {
    double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    double sq_sum = std::inner_product(values.begin(), values.end(), values.begin(), 0.0,
                                       std::plus<>(),
                                       [mean](NODE_T a, NODE_T b) { return (a-mean)*(b-mean); });
    return std::sqrt(sq_sum / values.size());
}

//for primal dual edge cover
void calcDegree(WeightEdgeSimList & edgeList, std::vector<NODE_T> &dV) {

  for(auto we:edgeList) {
    dV[we.u]++;
    dV[we.v]++; 
  }
}
