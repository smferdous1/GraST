# GraST

GrasT is a C++ implementation of some streaming graph algorithms. Currently, GraST implements two graph problems: 1) approximate maximum weight matching and 2) approximate minimum weight edge cover. GraST supports two ways to simulate streams: 1) edge read one by one from a matrix market (MTX) format file, which is used to report memory and overall runtime performance, and 2) read all the edges from a file (only support mtx format for now) and then stream from memory, which is used to compare the algorithmic time to the offline algorithms. Please see the following paper for details on the implementation and results for streaming matching and edge cover. Also, if you use GraST, please cite this paper.

    @InProceedings{ferdous_et_al:LIPIcs.SEA.2024.12,
    author =	{Ferdous, S M and Pothen, Alex and Halappanavar, Mahantesh},
    title =	{{Streaming Matching and Edge Cover in Practice}},
    booktitle =	{22nd International Symposium on Experimental Algorithms (SEA 2024)},
    pages =	{12:1--12:22},
    series =	{Leibniz International Proceedings in Informatics (LIPIcs)},
    ISBN =	{978-3-95977-325-6},
    ISSN =	{1868-8969},
    year =	{2024},
    volume =	{301},
    editor =	{Liberti, Leo},
    publisher =	{Schloss Dagstuhl -- Leibniz-Zentrum f{\"u}r Informatik},
    address =	{Dagstuhl, Germany},
    URL =		{https://drops.dagstuhl.de/entities/document/10.4230/LIPIcs.SEA.2024.12},
    URN =		{urn:nbn:de:0030-drops-203773},
    doi =		{10.4230/LIPIcs.SEA.2024.12},
    annote =	{Keywords: Matching, Edge Cover, Semi-Streaming Algorithm, Parallel Algorithms, Algorithm Engineering}
  }
  ## Description
  The src directory contains the implementation of the streaming matching, edge covering, and few auxiliary algorithms. The [StreamMatch.cc](./src/StreamMatch.cc) source file implements two semi-streaming matching algorithms: The $\frac{1}{2+\epsilon}$ Paz and Swartzman [2], and the $1/6$-approximate due to Feigenbaum et al [1]. The [StreamEC.cc](src/StreamEC.cc) file has the implementation of three new streaming edge cover algorithm. See the detailed description of these algorithms in
    
 ## Compilation
 1. Clone the GrasT github repo
    `git clone https://github.com/smferdous1/GraST-copy.git` 
 2.  Create a build directory and change current directory to the build one: `mkdir -p build && cd build`
 3. Generate the make files: `cmake ..`
 4. Generate the binaries: `make`
 
 This should create several binaries in the `build/apps` directory.  The **stmatch** and **stec** are the two binaries that can be used to execte the streaming matching and edge covering algorithms, respectively. Executing these binaires with **-h** flag provides with detailed description of the usage. 

## k-disjoint Matching
The streaming k-disjoint matching implementation are provided in the "kdm" branch. This implement two streaming algorithm from our following paper.

   @InProceedings{ferdous_et_al:LIPIcs.ESA.2024.53,
   author =	{Ferdous, S M and Samineni, Bhargav and Pothen, Alex and Halappanavar, Mahantesh and Krishnamoorthy, Bala},
   title =	{{Semi-Streaming Algorithms for Weighted k-Disjoint Matchings}},
   booktitle =	{32nd Annual European Symposium on Algorithms (ESA 2024)},
   pages =	{53:1--53:19},
   series =	{Leibniz International Proceedings in Informatics (LIPIcs)},
   ISBN =	{978-3-95977-338-6},
   ISSN =	{1868-8969},
   year =	{2024},
   volume =	{308},
   editor =	{Chan, Timothy and Fischer, Johannes and Iacono, John and Herman, Grzegorz},
   publisher =	{Schloss Dagstuhl -- Leibniz-Zentrum f{\"u}r Informatik},
   address =	{Dagstuhl, Germany},
   URL =		{https://drops.dagstuhl.de/entities/document/10.4230/LIPIcs.ESA.2024.53},
   URN =		{urn:nbn:de:0030-drops-211245},
   doi =		{10.4230/LIPIcs.ESA.2024.53},
   annote =	{Keywords: Matchings, Semi-Streaming Algorithms, Approximation Algorithms}
   }

We have two algorithms implementation here: i) primal-dual based method (**stk**) and ii) a b-matching reduction based method (**stkb**). The b-matching based method and the necessary codes are implemented by Bhargav Samineni. 

The most important binary is **kstmatch** which runs these algorithms. Please use **-h** flag to see the detailed description of the usage. 

We have also implemented the streaming b-matching due to Huang and Sellier algorithm and edge coloring algorithm due to Misra and Gries [4], which are needed for **stkb**. A DP based post-processing approach is also used to improve the weights (Use **--dp** as flags.) 

## References
[1] Joan Feigenbaum, Sampath Kannan, Andrew McGregor, Siddharth Suri, and Jian Zhang. On graph problems in a semi-streaming model. Theor. Comput. Sci., 348(2-3):207–216, 2005. doi:10.1016/j.tcs.2005.09.013.

[2] Ami Paz and Gregory Schwartzman. A (2 + ε)-approximation for maximum weight matching in
the semi-streaming model. In Proceedings of the ACM-SIAM Symposium on Discrete Algorithms
(SODA), pages 2153–2161, 2017

[3] Huang, Chien-Chung, and François Sellier. "Semi-streaming algorithms for submodular function maximization under b-matching constraint." Approximation, Randomization, and Combinatorial Optimization. Algorithms and Techniques (APPROX/RANDOM 2021). Schloss Dagstuhl–Leibniz-Zentrum für Informatik, 2021.

[4][ Misra & Gries edge-coloring algorithm](https://en.wikipedia.org/wiki/Misra_%26_Gries_edge-coloring_algorithm)

Please contact S M Ferdous (sm.ferdous@pnnl.gov or ferdous.csebuet@gmail.com) if you have any questions!
