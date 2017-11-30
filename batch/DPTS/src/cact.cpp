/*
 *	Author: Cheng Long
 *	Email: clong@cse.ust.hk
 */



#include "cact.h"


/*
 *
 */
tra_set_t* tra_set_alloc( int tra_n)
{
	tra_set_t* tra_set_v;

	tra_set_v = ( tra_set_t*)malloc( sizeof( tra_set_t));
	memset( tra_set_v, 0, sizeof( tra_set_t));

	tra_set_v->tra_n = tra_n;
	tra_set_v->tra_array = ( tra_list_t**)malloc( sizeof( tra_list_t*) * tra_n );
	memset( tra_set_v->tra_array, 0, sizeof( tra_list_t*) * tra_n);

	return tra_set_v;
}

/*
 *	
 */
void tra_set_release( tra_set_t* tra_set_v)
{
	int i;

	if( tra_set_v == NULL)
		return;

	for( i=0; i<tra_set_v->tra_n; i++)
		tra_list_release( tra_set_v->tra_array[ i]);

	free( tra_set_v->tra_array);
	free( tra_set_v);
}

/*
 *	"Spatial Decaying Function"
 */
float calc_f_eps( loc_t* loc_v1, loc_t* loc_v2, float eps)
{
	float dist;

	dist = ( float)calc_Euclidean_dist( loc_v1, loc_v2);

	if( dist > eps)
		return 0;
	else
		return 1 - dist / eps;
}

/*
 *	Compute the "clue scores".
 */
float calc_score_eps_tau( tra_node_t* tra_node_v, tra_list_t* tra_list_v, float eps, float tau)
{
	float score_max, score_v;	
	tra_node_t* iter;

	float t_sta, t_end;

	t_sta = tra_node_v->tri_v.sec - tau;
	t_end = tra_node_v->tri_v.sec + tau;

	score_max = 0;
	iter = tra_list_v->head->next;
	while( iter != NULL)
	{
		if( iter->tri_v.sec > t_end)
			break;

		if( iter->tri_v.sec >= t_sta &&
			iter->tri_v.sec <= t_end)
		{
			score_v = calc_f_eps( &tra_node_v->tri_v.loc_v, &iter->tri_v.loc_v, eps);

			if( score_v > score_max)
				score_max = score_v;
		}

		iter = iter->next;
	}

	return score_max;
}

/*
 *	Calculate the CATS similarity between two trajectories.
 */
float calc_cats( tra_list_t* tra_list_v1, tra_list_t* tra_list_v2, float eps, float tau)
{
	float score_sum;
	tra_node_t* iter;

	score_sum = 0;
	iter = tra_list_v1->head->next;
	while( iter != NULL)
	{
		score_sum += calc_score_eps_tau( iter, tra_list_v2, eps, tau);
		
		iter = iter->next;
	}

	return score_sum / tra_list_v1->pos_n;
}


/*
 *
 */
c_graph_t* c_graph_alloc( int tra_n)
{
	int i;
	c_graph_t* c_graph_v;

	c_graph_v = ( c_graph_t*)malloc( sizeof( c_graph_t));
	memset( c_graph_v, 0, sizeof( c_graph_t));

	c_graph_v->num = tra_n;
	c_graph_v->matrix = ( float**)malloc( sizeof( float*) * tra_n);
	memset( c_graph_v->matrix, 0, sizeof( float*) * tra_n);

	for( i=0; i<tra_n; i++)
	{
		c_graph_v->matrix[ i] = ( float*)malloc( sizeof( float) * tra_n);
		memset( c_graph_v->matrix[ i], 0, sizeof( float) * tra_n);
	}

	return c_graph_v;
}

/*
 *
 */
void c_graph_release( c_graph_t* c_graph_v)
{
	int i;

	if( c_graph_v == NULL)
		return;
	
	for( i=0; i<c_graph_v->num; i++)
		free( c_graph_v->matrix[ i]);
	
	free( c_graph_v->matrix);

	free( c_graph_v);
}

/*
 *	Generate the Clue-Graph.
 */
c_graph_t* clue_graph_gen( tra_set_t* tra_set_v, float lambda, float eps, float tau)
{
	int i, j, tra_n;
	float cats_v;
	c_graph_t* c_graph_v;
	tra_list_t** tra_array;


	tra_n = tra_set_v->tra_n;
	tra_array = tra_set_v->tra_array;

	c_graph_v = c_graph_alloc( tra_n);

	for( i=0; i<tra_n; i++)
	{
		for( j=0; j<tra_n; j++)
		{
			if( j == i)
				continue;

			cats_v = calc_cats( tra_array[ i], tra_array[ j], eps, tau);
			if( cats_v >= lambda)
				c_graph_v->matrix[ i][ j] = cats_v;
		}
	}
	
	return c_graph_v;
}

/*
 *
 */
sc_graph_t* sc_graph_alloc( int tra_n)
{
	int i;
	sc_graph_t* sc_graph_v;

	sc_graph_v = ( sc_graph_t*)malloc( sizeof( sc_graph_t));
	memset( sc_graph_v, 0, sizeof( sc_graph_t));

	sc_graph_v->num = tra_n;
	sc_graph_v->matrix = ( int**)malloc( sizeof( int*) * tra_n);
	memset( sc_graph_v->matrix, 0, sizeof( int*) * tra_n);

	for( i=0; i<tra_n; i++)
	{
		sc_graph_v->matrix[ i] = ( int*)malloc( sizeof( int) * tra_n);
		memset( sc_graph_v->matrix[ i], 0, sizeof( int) * tra_n);
	}

	return sc_graph_v;
}

/*
 *
 */
void sc_graph_release( sc_graph_t* sc_graph_v)
{
	int i;

	if( sc_graph_v == NULL)
		return;
	
	for( i=0; i<sc_graph_v->num; i++)
		free( sc_graph_v->matrix[ i]);
	
	free( sc_graph_v->matrix);

	free( sc_graph_v);
}

/*
 *
 */
sc_graph_t* strong_clue_graph_gen( c_graph_t* c_graph_v)
{
	int i, j, num;
	sc_graph_t* sc_graph_v;

	num = c_graph_v->num;

	sc_graph_v = sc_graph_alloc( num);
	
	for( i=0; i<num; i++)
	{
		for( j=i+1; j<num; j++)
		{
			if( c_graph_v->matrix[ i][ j] > 0 &&
				c_graph_v->matrix[ j][ i] > 0)
			{
				sc_graph_v->matrix[ i][ j] = 1;
				sc_graph_v->matrix[ j][ i] = 1;
			}
		}
	}

	return sc_graph_v;
}

/*
 *
 */
c_node_t* c_node_alloc_and_ini( int id)
{
	c_node_t* c_node_v;

	c_node_v = ( c_node_t*)malloc( sizeof( c_node_t));
	memset( c_node_v, 0, sizeof( c_node_t));

	c_node_v->id = id;

	return c_node_v;
}

/*
 *
 */
c_list_t* c_list_alloc( )
{
	c_list_t* c_list_v;

	c_list_v = ( c_list_t*)malloc( sizeof( c_list_t));
	memset( c_list_v, 0, sizeof( c_list_t));

	c_list_v->head = ( c_node_t*)malloc( sizeof( c_node_t));
	memset( c_list_v->head, 0, sizeof( c_node_t));

	return c_list_v;
}

/*
 *
 */
void add_c_list_entry( c_list_t* c_list_v, int id)
{
	c_node_t* c_node_v;

	c_node_v = c_node_alloc_and_ini( id);
	
	c_node_v->next = c_list_v->head->next;
	c_list_v->head->next = c_node_v;

	c_list_v->size ++;
}
	

/*
 *
 */
void c_list_release( c_list_t* c_list_v)
{
	c_node_t* iter, *tmp;

	if( c_list_v == NULL)
		return;

	iter = c_list_v->head;
	while( iter != NULL)
	{
		tmp = iter->next;
		free( iter);

		iter = tmp;
	}

	free( c_list_v);
}

/*
 *
 */
clus_list_t* clus_list_alloc( )
{
	clus_list_t* clus_list_v;

	clus_list_v = ( clus_list_t*)malloc( sizeof( clus_list_t));
	memset( clus_list_v, 0, sizeof( clus_list_t));

	clus_list_v->head = ( clus_node_t*)malloc( sizeof( clus_node_t));
	memset( clus_list_v->head, 0, sizeof( clus_node_t));
	clus_list_v->head->clique = c_list_alloc( );

	clus_list_v->rear = clus_list_v->head;

	return clus_list_v;
}

/*
 *	
 */
void add_clus_list_entry( clus_list_t* clus_list_v, c_list_t* c_list_v)
{
	clus_list_v->rear->next = ( clus_node_t*)malloc( sizeof( clus_node_t));
	memset( clus_list_v->rear->next, 0, sizeof( clus_node_t));

	clus_list_v->rear->next->clique = c_list_v;
	
	clus_list_v->rear = clus_list_v->rear->next;
	clus_list_v->size ++;

	return;
}

/*
 *
 */
void clus_list_release( clus_list_t* clus_list_v)
{
	clus_node_t* iter, *tmp;

	if( clus_list_v == NULL)
		return;

	iter = clus_list_v->head;
	while( iter != NULL)
	{
		tmp = iter->next;

		free( iter);

		iter = tmp;
	}

	free( clus_list_v);
}

/*
 *
 */
clus_list_t* comp_clique_cover( sc_graph_t* sc_graph_v)
{
	int i, ver_n, v_1, v_2;
	clus_list_t* clique_cover;
	clus_node_t* clus_node_v;
	c_list_t* c_list_v;
	c_node_t* c_node_v;


	ver_n = sc_graph_v->num;

	clique_cover = clus_list_alloc( );

	//
	i = 0;

	c_list_v = c_list_alloc( );
	add_c_list_entry( c_list_v, i);
	add_clus_list_entry( clique_cover, c_list_v);

	for( i=1; i<ver_n; i++)
	{
		v_1 = i;

		//
		clus_node_v = clique_cover->head->next;
		while( clus_node_v != NULL)
		{
			c_list_v = clus_node_v->clique;

			c_node_v = c_list_v->head->next;
			while( c_node_v != NULL)
			{
				v_2 = c_node_v->id;

				if( sc_graph_v->matrix[ v_1][ v_2] == 0)
					break;

				c_node_v = c_node_v->next;
			}

			if( c_node_v == NULL)
			{
				//vertex v_1 could be added to the current clique c_list_v.
				add_c_list_entry( c_list_v, v_1);
				break;
			}			
			
			clus_node_v = clus_node_v->next;
		}

		if( clus_node_v == NULL)
		{
			//Create a new clique for v_1.
			c_list_v = c_list_alloc( );
			add_c_list_entry( c_list_v, v_1);
			add_clus_list_entry( clique_cover, c_list_v);
		}
	}//for(i).

	return clique_cover;
}

/*
 *
 */
float is_clue_connected( c_list_t* c_list_v1, c_list_t* c_list_v2, c_graph_t* c_graph_v, float lambda, float** extra_matrix)
{
	int i, ver_n, i_tag, v_1, v_2;
	float gap;

	c_node_t* iter1, *iter2;

	i_tag = 0;
	ver_n = c_graph_v->num;
	
	if( extra_matrix == NULL)
	{
		//No extra edges.
		i_tag = 1;

		extra_matrix = ( float**)malloc( sizeof( float*) * ver_n);
		memset( extra_matrix, 0, sizeof( float*) * ver_n);

		for( i=0; i<ver_n; i++)
		{
			extra_matrix[ i] = ( float*)malloc( sizeof( float) * ver_n);
			memset( extra_matrix[ i], 0, sizeof( float) * ver_n);
		}
	}

	//
	gap = 0;
	iter1 = c_list_v1->head->next;
	while( iter1 != NULL)
	{
		v_1 = iter1->id;
		
		iter2 = c_list_v2->head->next;
		while( iter2 != NULL)
		{
			v_2 = iter2->id;

			if( c_graph_v->matrix[ v_1][ v_2] > 0 ||
				extra_matrix[ v_1][ v_2] > 0)
				break;

			iter2 = iter2->next;
		}

		if( iter2 == NULL)
		{
			//Extra edges are needed.
			v_2 = c_list_v2->head->next->id;
			extra_matrix[ v_1][ v_2] = lambda;

			gap += lambda;
		}

		iter1 = iter1->next;
	}//
		

	if( i_tag == 1)
	{
		for( i=0; i<ver_n; i++)
			free( extra_matrix[ i]);

		free( extra_matrix);
	}

	return gap;
}

/*
 *
 */
float comp_CCOH( clus_list_t* clus_list_v, c_graph_t* c_graph_v, float lambda)
{
	int i, ver_n;
	float gap_sum;
	float** extra_matrix;

	clus_node_t* iter1, *iter2;
	c_list_t* c_list_v1, *c_list_v2;

	ver_n = c_graph_v->num;

	//
	extra_matrix = ( float**)malloc( sizeof( float*) * ver_n);
	memset( extra_matrix, 0, sizeof( float*) * ver_n);

	for( i=0; i<ver_n; i++)
	{
		extra_matrix[ i] = ( float*)malloc( sizeof( float) * ver_n);
		memset( extra_matrix[ i], 0, sizeof( float) * ver_n);
	}

	//
	gap_sum = 0;
	iter1 = clus_list_v->head->next;
	while( iter1 != NULL)
	{
		c_list_v1 = iter1->clique;

		iter2 = clus_list_v->head->next;
		while( iter2 != NULL)
		{
			if( iter2 == iter1)
			{
				iter2 = iter2->next;
				continue;
			}

			c_list_v2 = iter2->clique;

			gap_sum += is_clue_connected( c_list_v1, c_list_v2, c_graph_v, lambda, extra_matrix);

			iter2 = iter2->next;
		}

		iter1 = iter1->next;
	}

	for( i=0; i<ver_n; i++)
		free( extra_matrix[ i]);
	free( extra_matrix);

	return gap_sum;
}

/*
 *
 */
float comp_CSEP( clus_list_t* clus_list_v1, clus_list_t* clus_list_v2, c_graph_t* c_graph_v)
{
	float weight_sum;
	clus_node_t* iter1, *iter2;
	c_list_t* c_list_v1, *c_list_v2;
	c_node_t* c_node_v1, *c_node_v2;

	weight_sum = 0;
	iter1 = clus_list_v1->head->next;
	while( iter1 != NULL)
	{
		c_list_v1 = iter1->clique;

		c_node_v1 = c_list_v1->head->next;
		while( c_node_v1 != NULL)
		{			
			iter2 = clus_list_v2->head->next;
			while( iter2 != NULL)
			{
				c_list_v2 = iter2->clique;

				c_node_v2 = c_list_v2->head->next;
				while( c_node_v2 != NULL)
				{
					weight_sum += c_graph_v->matrix[ c_node_v1->id][ c_node_v2->id];
					
					c_node_v2 = c_node_v2->next;
				}

				iter2 = iter2->next;
			}

			c_node_v1 =  c_node_v1->next;
		}

		iter1 = iter1->next;
	}

	return weight_sum;
}

/*
 *
 */
float comp_benefit( clus_list_t* clus_list_v1, clus_list_t* clus_list_v2, c_graph_t* c_graph_v, float lambda)
{
	float benefit, DesCSEP, IncCCOH, I_min, I_v;

	clus_node_t* iter1, *iter2;
	c_list_t* c_list_v1, *c_list_v2;

	benefit = 0;

	//DesCSEP.
	DesCSEP = ( comp_CSEP( clus_list_v1, clus_list_v2, c_graph_v) + 
				comp_CSEP( clus_list_v2, clus_list_v1, c_graph_v)) / 2;

	//IncCCOH.
	IncCCOH = comp_CCOH( clus_list_v1, c_graph_v, lambda) + 
			  comp_CCOH( clus_list_v2, c_graph_v, lambda);
	
	iter1 = clus_list_v1->head->next;
	while( iter1 != NULL)
	{
		c_list_v1 = iter1->clique;

		I_min = FLT_MAX;

		iter2 = clus_list_v2->head->next;
		while( iter2 != NULL)
		{
			c_list_v2 = iter2->clique;

			I_v = is_clue_connected( c_list_v1, c_list_v2, c_graph_v, lambda, NULL);

			if( I_v < I_min)
				I_min = I_v;

			iter2 = iter2->next;
		}

		IncCCOH += I_min;

		iter1 = iter1->next;
	}

	return DesCSEP - IncCCOH;
}

/*
 *
 */
clus_set_t* clus_set_alloc( int clus_n)
{
	clus_set_t* clus_set_v;

	clus_set_v = ( clus_set_t*)malloc( sizeof( clus_set_t));
	memset( clus_set_v, 0, sizeof( clus_set_t));

	clus_set_v->clus_array = ( clus_list_t**)malloc( sizeof( clus_list_t*) * clus_n);
	memset( clus_set_v->clus_array, 0, sizeof( clus_list_t*) * clus_n);

	clus_set_v->clus_n = clus_n;

	return clus_set_v;
}

/*
 *
 */
void clus_set_release( clus_set_t* clus_set_v)
{
	int i;

	if( clus_set_v == NULL)
		return;

	for( i=0; i<clus_set_v->clus_n; i++)
		clus_list_release( clus_set_v->clus_array[ i]);

	free( clus_set_v);

	return;
}

/*
 *	Hierarchical clustering.
 */
clus_set_t* cats( tra_set_t* tra_set_v, float eps, float tau, float lambda)
{
	int i, j, clique_n, m_1, m_2, clus_n, clus_cnt;
	int* tag;
	float benefit_max;
	float** benefit_matrix;

	clus_set_t* clus_set_v;

	c_graph_t* c_graph_v;
	sc_graph_t* sc_graph_v;
	clus_list_t* clique_cover;
	clus_list_t** clus_array;
	clus_node_t* clus_node_v;

	//clus_set_v = clus_set_alloc( 

	//Step 1: Construct a clue-graph.
	printf( "Constructing a Clue-Graph ...\n");
	c_graph_v = clue_graph_gen( tra_set_v, lambda, eps, tau);

	sc_graph_v = strong_clue_graph_gen( c_graph_v);

	//Step 2: Compute a clique cover.
	printf( "Computing the Cique Cover ...\n");
	clique_cover = comp_clique_cover( sc_graph_v);

	//Step 3: Hierarchical clustering.
	clique_n = clique_cover->size;
	clus_n = clique_n;
	
	tag = ( int*)malloc( sizeof( int) * clique_n);
	memset( tag, 0, sizeof( int) * clique_n);

	benefit_matrix = ( float**)malloc( sizeof( float*) * clique_n);
	memset( benefit_matrix, 0, sizeof( float*) * clique_n);

	for( i=0; i<clique_n; i++)
	{
		benefit_matrix[ i] = ( float*)malloc( sizeof( float) * clique_n);
		memset( benefit_matrix[ i], 0, sizeof( float) * clique_n);
	}

	//
	clus_array = ( clus_list_t**)malloc( sizeof( clus_list_t*) * clique_n);
	memset( clus_array, 0, sizeof( clus_list_t*) * clique_n);

	clus_node_v = clique_cover->head->next;
	for( i=0; i<clique_n; i++)
	{
		clus_array[ i] = clus_list_alloc( );

		add_clus_list_entry( clus_array[ i], clus_node_v->clique);

		clus_node_v->clique = NULL;

		clus_node_v = clus_node_v->next;
	}

	benefit_max = -1;
	m_1 = 0;
	m_2 = 0;
	for( i=0; i<clique_n; i++)
	{
		for( j=i+1; j<clique_n; j++)
		{
			benefit_matrix[ i][ j] = comp_benefit( clus_array[ i], clus_array[ j], c_graph_v, lambda);

			if( benefit_matrix[ i][ j] > benefit_max)
			{
				benefit_max = benefit_matrix[ i][ j];
				m_1 = i;
				m_2 = j;
			}
		}
	}

	while( benefit_max > 0)
	{
		//Merge.
		printf( "Merging ...\n");
		clus_array[ m_1]->rear->next = clus_array[ m_2]->head->next;
		clus_array[ m_1]->size += clus_array[ m_2]->size;

		clus_array[ m_2]->head->next = NULL;
		clus_array[ m_2]->size = 0;

		tag[ m_2] = 1;

		clus_n --;

		//Update the benefits.
		for( i=0; i<m_1; i++)
		{
			if( tag[ i] == 0)
				benefit_matrix[ i][ m_1] = comp_benefit( clus_array[ i], clus_array[ m_1], c_graph_v, lambda);
		}
		for( j=m_1+1; j<clique_n; j++)
		{
			if( tag[ j] == 0)
				benefit_matrix[ m_1][ j] = comp_benefit( clus_array[ m_1], clus_array[ j], c_graph_v, lambda);
		}

		
		//
		benefit_max = -1;
		for( i=0; i<clique_n; i++)
		{
			if( tag[ i] == 1)
				continue;

			for( j=i+1; j<clique_n; j++)
			{
				if( tag[ j] == 1)
					continue;

				if( benefit_matrix[ i][ j] > benefit_max)
				{
					benefit_max = benefit_matrix[ i][ j];
					m_1 = i;
					m_2 = j;
				}
			}
		}
	}//while

	//Construct clus_set_v;
	clus_cnt = 0;
	clus_set_v = clus_set_alloc( clus_n);
	clus_set_v->tra_n = tra_set_v->tra_n;
	for( i=0; i<clique_n; i++)
	{
		if( tag[ i] == 0)
		{
			clus_set_v->clus_array[ clus_cnt++] = clus_array[ i];
			clus_array[ i] = NULL;
		}
	}

	//t
	if( clus_cnt != clus_n)
	{
		fprintf( stderr, "clus_n inconsistency [cats].\n");
		exit( 0);
	}		

	//Release the resources.
	free( tag);
	for( i=0; i<clique_n; i++)
	{
		free( benefit_matrix[ i]);
		clus_list_release( clus_array[ i]);
	}

	free( benefit_matrix);
	free( clus_array);	

	clus_list_release( clique_cover);
	sc_graph_release( sc_graph_v);
	c_graph_release( c_graph_v);

	return clus_set_v;
}

/*
 *
 */
tra_set_t* emp_read_tra_set( char* f_name)
{
	int i, dim, tra_n;

	FILE* i_fp;
	tra_set_t* tra_set_v;

	if( ( i_fp = fopen( f_name, "r")) == NULL)
	{
		fprintf( stderr, "Cannot open %s [emp_read_tra_set]\n", f_name);
		exit( 0);
	}
	
	fscanf( i_fp, "%i%i", &dim, &tra_n);

	tra_set_v = tra_set_alloc( tra_n);
	
	for( i=0; i<tra_n; i++)
		tra_set_v->tra_array[ i] = read_trajectory_v2( i_fp);

	fclose( i_fp);

	return tra_set_v;
}

/*
 *
 */
void emp_print_clus_set( clus_set_t* clus_set_v, char* f_name)
{
	int i;
	int* clus_member;

	clus_node_t* iter;
	c_node_t* c_node_v;

	FILE* o_fp;

	if( ( o_fp = fopen( f_name, "w")) == NULL)
	{
		fprintf( stderr, "Cannot open %s [emp_print_clus_set].\n");
		exit( 0);
	}

	//
	clus_member = ( int*)malloc( sizeof( int) * clus_set_v->tra_n);
	memset( clus_member, 0, sizeof( int) * clus_set_v->tra_n);

	for( i=0; i<clus_set_v->clus_n; i++)
	{
		iter = clus_set_v->clus_array[ i]->head->next;

		while( iter != NULL)
		{
			c_node_v = iter->clique->head->next;

			while( c_node_v != NULL)
			{
				clus_member[ c_node_v->id] = i;

				c_node_v = c_node_v->next;
			}

			iter = iter->next;
		}
	}
	
	//
	fprintf( o_fp, "%i\n%i\n", clus_set_v->tra_n, clus_set_v->clus_n);
	
	for( i=0; i<clus_set_v->tra_n; i++)
	{
		fprintf( o_fp, "%i  1  %i\n", i, clus_member[ i]);
	}

	//
	free( clus_member);
	fclose( o_fp);
}

/*
 *
 */
void emp_cats( )
{
	int ins_n;

	float eps, tau, lambda;
	char input_f_name[ MAX_FILENAME_LENG];
	char output_f_name[ MAX_FILENAME_LENG];
	char cluster_membership_f_name[ MAX_FILENAME_LENG];

	FILE *c_fp;

	tra_set_t* tra_set_v;
	clus_set_t* clus_set_v;
	
	if( ( c_fp = fopen( CATS_CONFIG, "r")) == NULL)
	{
		fprintf( stderr, "Cannot open %s.\n", CATS_CONFIG);
		exit( 0);
	}

	ins_n = 1;
	while( fscanf( c_fp, "%s%s%s%f%f%f", input_f_name, output_f_name, cluster_membership_f_name, 
										&eps, &tau, &lambda) != EOF)
	{
		printf( "Handling Case #%i ...\n", ins_n++);

		//Read the trajectory data.
		tra_set_v = emp_read_tra_set( input_f_name);

		//Perform trajectory clustering.
		clus_set_v = cats( tra_set_v, eps, tau, lambda);

		//Print the clustering results.
		emp_print_clus_set( clus_set_v, cluster_membership_f_name);

		//Release the resources.
		tra_set_release( tra_set_v);
		clus_set_release( clus_set_v);
	}

	fclose( c_fp);
	return;
}

/*
 *
 */
void collect_statistics_clustering_datasets( )
{
	int i, ins_n;

	//float eps, tau, lambda;
	char input_f_name[ MAX_FILENAME_LENG];
//	char output_f_name[ MAX_FILENAME_LENG];
//	char cluster_membership_f_name[ MAX_FILENAME_LENG];

	int pos_n_cnt, tra_n_cnt, dir_n_cnt;
	int *pos_n_array;
	float pos_n_mean, pos_n_dev, angle_v, dir_mean, dir_dev, dir_sum;
	float* dir_array;

	FILE *c_fp, *s_fp;

	tra_set_t* tra_set_v;
	//clus_set_t* clus_set_v;
	//tra_node_t* tra_node_v;
	tra_list_t* tra_list_v;
	R_list_t* R_list_v;
	R_node_t* R_node_v1, *R_node_v2;
	
	if( ( c_fp = fopen( DATA_CONFIG_FILE_V2, "r")) == NULL ||
		( s_fp = fopen( DATA_STATISTICS_FILE, "w")) == NULL)
	{
		fprintf( stderr, "Cannot open the config/stat.\n");
		exit( 0);
	}

	ins_n = 1;
	while( fscanf( c_fp, "%s", input_f_name) != EOF)
	{
		printf( "Handling Case #%i ...\n", ins_n++);

		//Read the trajectory data.
		tra_set_v = emp_read_tra_set( input_f_name);

		tra_n_cnt = tra_set_v->tra_n;
		pos_n_cnt = 0;
		for( i=0; i<tra_set_v->tra_n; i++)
		{
			pos_n_cnt += tra_set_v->tra_array[ i]->pos_n;
		}

		pos_n_array = ( int*)malloc( sizeof( int) * tra_n_cnt);
		memset( pos_n_array, 0, sizeof( int) * tra_n_cnt);

		dir_array = ( float*)malloc( sizeof( float) * pos_n_cnt);
		memset( dir_array, 0, sizeof( float) * pos_n_cnt);		

		dir_sum = 0;
		dir_n_cnt = 0;
		for( i=0; i<tra_set_v->tra_n; i++)
		{
			tra_list_v = tra_set_v->tra_array[ i];
			
			if( tra_list_v->pos_n > 2)
			{
				R_list_v = R_list_transform( tra_list_v);
				
				R_node_v1 = R_list_v->head->next;
				R_node_v2 = R_node_v1->next;
				while( R_node_v2 != NULL)
				{
					angle_v = R_node_v1->slope - R_node_v2->slope;
					
					//if( errno)
					//printf( "errno = %i\n", errno);
					
					//angle_v = angle_v >= 0 ? angle_v : -angle_v;
					if( angle_v < 0)
						angle_v = - angle_v;
					
					if( angle_v > 2 * PI - angle_v)
						angle_v = 2 * PI - angle_v;
					
					//angle_v = angle_v <= 2 * PI - angle_v ? angle_v : 2 * PI - angle_v;
					
					//printf( "%f\n", angle_v);
					
					if( angle_v >= 0 && angle_v < PI)
					{
						dir_sum += angle_v;
						dir_array[ dir_n_cnt++] = angle_v;
					}
					
					//
					//printf( "dir_sum: %lf\n", dir_sum);
					
					R_node_v1 = R_node_v2;
					R_node_v2 = R_node_v2->next;
				}//while
			}//if
		}//for(i)
		
		//Perform trajectory clustering.
		//clus_set_v = cats( tra_set_v, eps, tau, lambda);

		//Print the clustering results.
		//emp_print_clus_set( clus_set_v, cluster_membership_f_name);


		pos_n_mean = ( float)pos_n_cnt / tra_n_cnt;

		pos_n_dev = 0;
		for( i=0; i<tra_n_cnt; i++)
		{
			pos_n_dev += pow( pos_n_array[ i] - pos_n_mean, 2);
		}

		pos_n_dev /= tra_n_cnt;

		pos_n_dev = sqrt( pos_n_dev);
			
		
		//
		printf( "dir_sum: %f\n", dir_sum);
		if( dir_n_cnt != 0)
			dir_mean = dir_sum / dir_n_cnt;
		else
			dir_mean = 0;

		dir_dev = 0;
		for( i=0; i<dir_n_cnt; i++)
		{
			dir_dev += pow( dir_array[ i] - dir_mean, 2);
		}

		if( dir_n_cnt != 0)
		{
			dir_dev /= dir_n_cnt;
			dir_dev = sqrt( dir_dev);
		}
		else
			dir_dev = 0;

		//
		fprintf( s_fp, "%i\n%i\n%f\n%f\n\n", pos_n_cnt, tra_n_cnt, pos_n_mean, pos_n_dev);
		fprintf( s_fp, "%f\n%i\n%f\n%f\n\n", dir_sum, dir_n_cnt, dir_mean, dir_dev);

		fprintf( stdout, "%i\n%i\n%f\n%f\n\n", pos_n_cnt, tra_n_cnt, pos_n_mean, pos_n_dev);
		fprintf( stdout, "%lf\n%i\n%f\n%f\n\n", dir_sum, dir_n_cnt, dir_mean, dir_dev);

		//Release the resources.
		tra_set_release( tra_set_v);
		free( pos_n_array);
		free( dir_array);
		//clus_set_release( clus_set_v);
	}

	fclose( c_fp);
	fclose( s_fp);
	return;
}
