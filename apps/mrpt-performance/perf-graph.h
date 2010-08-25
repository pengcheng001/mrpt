/* +---------------------------------------------------------------------------+
   |          The Mobile Robot Programming Toolkit (MRPT) C++ library          |
   |                                                                           |
   |                   http://mrpt.sourceforge.net/                            |
   |                                                                           |
   |   Copyright (C) 2005-2010  University of Malaga                           |
   |                                                                           |
   |    This software was written by the Machine Perception and Intelligent    |
   |      Robotics Lab, University of Malaga (Spain).                          |
   |    Contact: Jose-Luis Blanco  <jlblanco@ctima.uma.es>                     |
   |                                                                           |
   |  This file is part of the MRPT project.                                   |
   |                                                                           |
   |     MRPT is free software: you can redistribute it and/or modify          |
   |     it under the terms of the GNU General Public License as published by  |
   |     the Free Software Foundation, either version 3 of the License, or     |
   |     (at your option) any later version.                                   |
   |                                                                           |
   |   MRPT is distributed in the hope that it will be useful,                 |
   |     but WITHOUT ANY WARRANTY; without even the implied warranty of        |
   |     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         |
   |     GNU General Public License for more details.                          |
   |                                                                           |
   |     You should have received a copy of the GNU General Public License     |
   |     along with MRPT.  If not, see <http://www.gnu.org/licenses/>.         |
   |                                                                           |
   +---------------------------------------------------------------------------+ */


#include <mrpt/math.h>
#include <mrpt/utils/CTicTac.h>

using namespace mrpt;
using namespace mrpt::utils;
using namespace mrpt::math;
using namespace mrpt::poses;
using namespace mrpt::random;
using namespace std;


// ------------------------------------------------------
//				Benchmark: Graphs
// ------------------------------------------------------

template <class EDGE_TYPE>
double graphs_test_populate(int nEdges, int _N)
{
	const long N = _N;

	std::vector< mrpt::poses::CNetworkOfPoses<EDGE_TYPE> > gs(N);

	CTicTac	 tictac;
	for (long i=0;i<N;i++)
	{
		mrpt::poses::CNetworkOfPoses<EDGE_TYPE>  &g = gs[i];
		for (int j=0;j<nEdges;++j)
		{
			g.insertEdge(j,j+1, EDGE_TYPE() );
		}
	}
	return tictac.Tac()/N;
}

template <class EDGE_TYPE>
double graphs_test_populate_at_end(int nEdges, int _N)
{
	const long N = _N;

	std::vector< mrpt::poses::CNetworkOfPoses<EDGE_TYPE> > gs(N);

	CTicTac	 tictac;
	for (long i=0;i<N;i++)
	{
		mrpt::poses::CNetworkOfPoses<EDGE_TYPE>  &g = gs[i];
		for (int j=0;j<nEdges;++j)
		{
			//g.insertEdgeAtEnd(j,j+1, EDGE_TYPE() );
			g.edges.insert(g.edges.end(),std::make_pair(std::make_pair(j,j+1), EDGE_TYPE() ));
		}
	}
	return tictac.Tac()/N;
}

template <class EDGE_TYPE, class MAPS_IMPLEMENTATION>
double graphs_dijkstra(int nNodes, int _N)
{
	const long N = _N;

	randomGenerator.randomize(111);
	// Generate random graph:
	mrpt::poses::CNetworkOfPoses<EDGE_TYPE,MAPS_IMPLEMENTATION> gs;
	{
		double edge_node_ratio = 2.0;
		for (unsigned int i=0;i<(unsigned int)nNodes;i++)
		{
			size_t nEdges = 1 + ( mrpt::random::randomGenerator.drawUniform32bit() % int(edge_node_ratio-1) );
			for (TNodeID k=0;k<nEdges;k++)
			{
				TNodeID dest;
				dest=i+1;
				if (k>0)
				{
					while (dest==i && dest!=i+1)	// Avoid self-loops!
						dest = mrpt::random::randomGenerator.drawUniform32bit()  % nNodes;
				}
				gs.insertEdge(i, dest, EDGE_TYPE() );
			}
		}
	}

	CTimeLogger tims;
	for (long i=0;i<N;i++)
	{
		tims.enter("op");
		mrpt::math::CDijkstra<EDGE_TYPE,MAPS_IMPLEMENTATION> dij(gs, TNodeID(0) );
		tims.leave("op");
		// Don't count the time of the destructor.
	}
	tims.enable(false);
	double ret = tims.getMeanTime("op");
	tims.clear();
	return ret;
}


// ------------------------------------------------------
// register_tests_graph
// ------------------------------------------------------
void register_tests_graph()
{
	randomGenerator.randomize(1234);

	lstTests.push_back( TestData("graph(2d): insertEdge x 1e3",graphs_test_populate<CPose2D>, 1e3,   100) );
	lstTests.push_back( TestData("graph(2d): insertEdgeAtEnd x 1e3",graphs_test_populate_at_end<CPose2D>, 1e3,   100) );
	lstTests.push_back( TestData("graph(2d pdf): insertEdge x 1e3",graphs_test_populate<CPosePDFGaussianInf>, 1e3,   100) );
	lstTests.push_back( TestData("graph(2d pdf): insertEdgeAtEnd x 1e3",graphs_test_populate_at_end<CPosePDFGaussianInf>, 1e3,   100) );

	lstTests.push_back( TestData("graph(2d): insertEdge x 1e4",graphs_test_populate<CPose2D>, 1e4,   25) );
	lstTests.push_back( TestData("graph(2d): insertEdgeAtEnd x 1e4",graphs_test_populate_at_end<CPose2D>, 1e4,   25) );
	lstTests.push_back( TestData("graph(2d pdf): insertEdge x 1e4",graphs_test_populate<CPosePDFGaussianInf>, 1e4,   25) );
	lstTests.push_back( TestData("graph(2d pdf): insertEdgeAtEnd x 1e4",graphs_test_populate_at_end<CPosePDFGaussianInf>, 1e4,   25) );

	lstTests.push_back( TestData("graph(3d): insertEdge x 1e3",graphs_test_populate<CPose3D>, 1e3,   100) );
	lstTests.push_back( TestData("graph(3d): insertEdgeAtEnd x 1e3",graphs_test_populate_at_end<CPose3D>, 1e3,   100) );
	lstTests.push_back( TestData("graph(3d pdf): insertEdge x 1e3",graphs_test_populate<CPose3DPDFGaussianInf>, 1e3,   100) );
	lstTests.push_back( TestData("graph(3d pdf): insertEdgeAtEnd x 1e3",graphs_test_populate_at_end<CPose3DPDFGaussianInf>, 1e3,   100) );

	lstTests.push_back( TestData("graph(3d): insertEdge x 1e4",graphs_test_populate<CPose3D>, 1e4,   25) );
	lstTests.push_back( TestData("graph(3d): insertEdgeAtEnd x 1e4",graphs_test_populate_at_end<CPose3D>, 1e4,   25) );
	lstTests.push_back( TestData("graph(3d pdf): insertEdge x 1e4",graphs_test_populate<CPose3DPDFGaussianInf>, 1e4,   25) );
	lstTests.push_back( TestData("graph(3d pdf): insertEdgeAtEnd x 1e4",graphs_test_populate_at_end<CPose3DPDFGaussianInf>, 1e4,   25) );


	lstTests.push_back( TestData("graph(3d,map): dijkstra 1e2 nodes",graphs_dijkstra<CPose3D,map_traits_stdmap>, 1e2, 50) );
	lstTests.push_back( TestData("graph(3d,map): dijkstra 1e3 nodes",graphs_dijkstra<CPose3D,map_traits_stdmap>, 1e3, 50) );
	lstTests.push_back( TestData("graph(3d,map): dijkstra 1e4 nodes",graphs_dijkstra<CPose3D,map_traits_stdmap>, 1e4, 5) );
//	  lstTests.push_back( TestData("graph(3d,vec): dijkstra 1e2 nodes",graphs_dijkstra<CPose3D,map_traits_map_as_vector>, 1e2, 50) );
//	lstTests.push_back( TestData("graph(3d,vec): dijkstra 1e3 nodes",graphs_dijkstra<CPose3D,map_traits_map_as_vector>, 1e3, 50) );
//	lstTests.push_back( TestData("graph(3d,vec): dijkstra 1e4 nodes",graphs_dijkstra<CPose3D,map_traits_map_as_vector>, 1e4, 5) );
}
