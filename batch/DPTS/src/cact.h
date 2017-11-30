/*
 *	Author: Cheng Long
 *	Email: clong@cse.ust.hk
 */



/*
 *	The implementation of the Clue-Aware Trajectory Clustering Algorithm (CATC) in 
 *	"Clustering and aggregating clues of trajectories for mining trajectory patterns and routes" by Chih-Chieh Hung et al.
 *
 *	By Cheng Long, April 2013.
 *	clong@cse.ust.hk.
 *
 */


#ifndef CACT_H
#define CACT_H


//#include "data_struct.h"
#include "RN_appro.h"


#define		CATS_CONFIG		"cats_config.txt"



//The data structure for storing a set of trajectories.
typedef struct tra_set
{
	int				tra_n;
	tra_list_t**	tra_array;

}	tra_set_t;


//The data structure for storing a clue_graph.
typedef struct c_graph
{
	int		num;
	float**	matrix;

}	c_graph_t;


//The data structure for storing a stronly 
typedef struct sc_graph
{
	int		num;
	int**	matrix;

}	sc_graph_t;


//The data structure for storing a clique.
typedef struct c_node
{
	int				id;
	struct	c_node* next;

}	c_node_t;

typedef struct c_list
{
	int			size;
	c_node_t*	head;
//	c_node_t*	rear;

}	c_list_t;


//The data structure for storing a cluster candidate, which is a set of cliques.
typedef struct clus_node
{
	c_list_t*			clique;
	struct clus_node*	next;

}	clus_node_t;


typedef struct clus_list
{
	int				size;
	clus_node_t*	head;
	clus_node_t*	rear;

}	clus_list_t;


//The data structure for storing a set of clusters.
typedef struct clus_set
{
	int				clus_n;
	int				tra_n;
	clus_list_t**	clus_array;

}	clus_set_t;

tra_set_t* tra_set_alloc( int tra_n);

void tra_set_release( tra_set_t* tra_set_v);

float calc_f_eps( loc_t* loc_v1, loc_t* loc_v2, float eps);

float calc_score_eps_tau( tra_node_t* tra_node_v, tra_list_t* tra_list_v, float eps, float tau);

float calc_cats( tra_list_t* tra_list_v1, tra_list_t* tra_list_v2, float eps, float tau);

c_graph_t* c_graph_alloc( int tra_n);

void c_graph_release( c_graph_t* c_graph_v);

c_graph_t* clue_graph_gen( tra_set_t* tra_set_v, float lambda, float eps, float tau);

sc_graph_t* sc_graph_alloc( int tra_n);

void sc_graph_release( sc_graph_t* sc_graph_v);

void c_list_release( c_list_t* c_list_v);

c_list_t* c_list_alloc( );

clus_list_t* clus_list_alloc( );

void clus_list_release( clus_list_t* clus_list_v);

sc_graph_t* strong_clue_graph_gen( c_graph_t* c_graph_v);

clus_list_t* comp_clique_cover( sc_graph_t* sc_graph_v);

float comp_CCOH( clus_list_t* clus_list_v, c_graph_t* c_graph_v, float lambda);

float comp_CSEP( clus_list_t* clus_list_v1, clus_list_t* clus_list_v2, c_graph_t* c_graph_v);

float comp_benefit( clus_list_t* clus_list_v1, clus_list_t* clus_list_v2, c_graph_t* c_graph_v, float lambda);

clus_set_t* clus_set_alloc( int clus_n);

void clus_set_release( clus_set_t* clus_set_v);

clus_set_t* cats( tra_set_t* tra_set_v, float eps, float tau, float lambda);

void emp_cats( );

void collect_statistics_clustering_datasets( );

#endif
