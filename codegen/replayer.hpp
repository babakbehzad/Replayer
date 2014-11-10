#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>

#ifndef __REPLAYER_HPP
#define __REPALYER_HPP
using namespace std;

typedef string (*func_t)(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, std::string args[], std::string ret_value[]);
typedef std::map<std::string, func_t> func_map_t;
typedef std::map<long, std::string> id_map_t;
typedef std::map<std::string, std::string> allocated_data_map_t;
typedef std::map<std::string, std::vector<int> > allocated_rank_map_t;

// File interface
string replay_H5Fcreate(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Fopen(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Fclose(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);

// Group interface
string replay_H5Gclose(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Gcreate1(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Gcreate2(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Gget_objinfo(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Giterate(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Gopen1(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Gopen2(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);

// Dataset interface
string replay_H5Dclose(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Dcreate1(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Dcreate2(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Dget_create_plist(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Dget_space(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Dget_type(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Dopen1(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) ;
string replay_H5Dopen2(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) ;
string replay_H5Dread(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Dwrite(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);

// Dataspace interface
string replay_H5Sclose(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Screate(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Screate_simple(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Sget_select_npoints(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) ;
string replay_H5Sget_simple_extent_dims(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Sget_simple_extent_npoints(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Sselect_elements(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Sselect_hyperslab(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Sselect_none(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);

// Datatype interface
string replay_H5Tclose(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Tcreate(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Topen1(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Topen2(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Tcommit1(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Tcommit2(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Tinsert(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Tcopy(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Tget_class(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Tget_size(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Tset_size(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);

// Attribute interface
string replay_H5Aclose(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Acreate1(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Acreate2(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Aget_name(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Aget_num_attrs(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Aget_space(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Aget_type(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Aopen(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Aopen_idx(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Aopen_name(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Aread(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Awrite(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);

// Property-List interface
string replay_H5Pclose(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Pcreate(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Pget_chunk(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Pget_mdc_config(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Pset_mdc_config(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Pset_alignment(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Pset_chunk(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Pset_dxpl_mpio(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Pset_fapl_core(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Pset_fapl_mpio(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Pset_fapl_mpiposix(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Pset_istore_k(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Pset_mdc_config(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) ;
string replay_H5Pset_meta_block_size(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);

// List interface
string replay_H5Lexists(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Lget_val(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Literate(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);

// Object interface
string replay_H5Oclose(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Oget_info(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Oget_info_by_name(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);
string replay_H5Oopen(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]);

string preamble();
string postamble();

#endif /*__REPLAYER_HPP*/
