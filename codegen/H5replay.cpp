#include "hdf5.h"

#include "replayer.hpp"

using namespace std;

extern id_map_t id_map;
extern int cur_id;
extern int dim_id;
extern int data_id;

extern allocated_data_map_t allocated_data;
extern allocated_rank_map_t allocated_rank;

// File interface
string replay_H5Fcreate(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	// for H5Fcreate
	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string filename = *tok_it;
	tok_it++;
	string flags = *tok_it;
	tok_it++;
	string fcpl_id = *tok_it;
	tok_it++;
	string fapl_id = *tok_it;

	long fcpl_id_int = boost::lexical_cast<long>(fcpl_id);
	string fcpl_id_str;
	if (fcpl_id_int == 0) {
		fcpl_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(fcpl_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << fcpl_id << "\" fcpl id was not found in the map, ERROR!" << std::endl;
		} else {
			fcpl_id_str = (it->second);
		}
	}

	long fapl_id_int = boost::lexical_cast<long>(fapl_id);
	string fapl_id_str;
	if (fapl_id_int == 0) {
		fapl_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(fapl_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << fapl_id << "\" fapl id was not found in the map, ERROR!" << std::endl;
		} else {
			fapl_id_str = (it->second);
		}
	}

	long ret_id = boost::lexical_cast<long>(ret_value[0]);
	string ret_id_str;
	/*
	id_map_t::const_iterator it = id_map.find(ret_id);
	if(it == id_map.end()) {
		std::cerr << "\"" << ret_value[0] << "\" return id was not found in the map, adding it" << std::endl;
	*/
	ostringstream ret_id_stream;
	ret_id_stream << "hid_" << cur_id;
	cur_id++;
	ret_id_str = ret_id_stream.str();
	id_map.insert(std::pair<long, string>(ret_id, ret_id_str));
	/*
	}
	else {
		ret_id_str = (it->second);
	}
	*/

	ostringstream output_stream;
	output_stream << "hid_t " << ret_id_str << " = H5Fcreate(\"" << filename << "\", " << flags << ", " << (fcpl_id_str == "0" ? "H5P_DEFAULT" : fcpl_id_str) << ", " << (fapl_id_str == "0" ? "H5P_DEFAULT" : fapl_id_str) << ");" << std::endl;
	return output_stream.str();
}

string replay_H5Fopen(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	// for H5Fopen
	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string filename = *tok_it;
	tok_it++;
	string flags = *tok_it;
	tok_it++;
	string fapl_id = *tok_it;

	long fapl_id_int = boost::lexical_cast<long>(fapl_id);
	string fapl_id_str;
	if (fapl_id_int == 0) {
		fapl_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(fapl_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << fapl_id << "\" fapl id was not found in the map, ERROR!" << std::endl;
		} else {
			fapl_id_str = (it->second);
		}
	}

	long ret_id = boost::lexical_cast<long>(ret_value[0]);
	string ret_id_str;
	id_map_t::const_iterator it = id_map.find(ret_id);
	if (it == id_map.end()) {
		std::cerr << "\"" << ret_value[0] << "\" return id was not found in the map, adding it" << std::endl;

		ostringstream ret_id_stream;
		ret_id_stream << "hid_" << cur_id;
		cur_id++;
		ret_id_str = ret_id_stream.str();
		id_map.insert(std::pair<long, string>(ret_id, ret_id_str));
	} else {
		ret_id_str = (it->second);
	}

	ostringstream output_stream;
	output_stream << "hid_t " << ret_id_str << " = H5Fopen(\"" << filename << "\", " << flags << ", " << (fapl_id_str == "0" ? "H5P_DEFAULT" : fapl_id_str) << ");" << std::endl;
	return output_stream.str();
}

string replay_H5Fclose(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	long file_id = boost::lexical_cast<long>(args[0]);

	string file_id_str;
	id_map_t::iterator it = id_map.find(file_id);
	if (it == id_map.end()) {
		std::cerr << "\"" << file_id << "\" file id was not found in the map, ERROR!" << std::endl;
	} else {
		file_id_str = (it->second);

		if (same) {
			id_map.erase(it);
		}
	}

	//fprintf(outputfh,"\n\tH5Fclose(%d);\n",file_id);
	ostringstream output_stream;
	output_stream << "H5Fclose(" << file_id_str << "); " << std::endl;
	return output_stream.str();
}

// Group interface
string replay_H5Gclose(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	long group_id = boost::lexical_cast<long>(args[0]);

	string group_id_str;
	id_map_t::iterator it = id_map.find(group_id);
	if (it == id_map.end()) {
		std::cerr << "\"" << group_id << "\" group id was not found in the map, ERROR?!" << std::endl;
		group_id_str = args[0];
	} else {
		group_id_str = (it->second);

		if (same) {
			id_map.erase(it);
		}
	}

	//fprintf(outputfh,"\n\tH5Gclose(%d);\n",group_id);
	ostringstream output_stream;
	output_stream << "H5Gclose(" << group_id_str << "); " << std::endl;
	return output_stream.str();
}

string replay_H5Gcreate1(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string loc_id = *tok_it;
	tok_it++;
	string name = *tok_it;
	tok_it++;
	string size_hint = *tok_it;

	long loc_id_int = boost::lexical_cast<long>(loc_id);
	string loc_id_str;
	if (loc_id_int == 0) {
		loc_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(loc_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << loc_id << "\" loc id was not found in the map, ERROR!" << std::endl;
		} else {
			loc_id_str = (it->second);
		}
	}

	long ret_id = boost::lexical_cast<long>(ret_value[0]);
	string ret_id_str;
	/*
	id_map_t::const_iterator it = id_map.find(ret_id);
	if(it == id_map.end()) {
		std::cerr << "\"" << ret_value[0] << "\" return id was not found in the map, adding it" << std::endl;
	*/
	ostringstream ret_id_stream;
	ret_id_stream << "hid_" << cur_id;
	cur_id++;
	ret_id_str = ret_id_stream.str();
	id_map.insert(std::pair<long, string>(ret_id, ret_id_str));
	/*
	}
	else {
		ret_id_str = (it->second);
	}
	*/

	ostringstream output_stream;
	output_stream << "hid_t " << ret_id_str << " = H5Gcreate1(" << loc_id_str << ", \"" << name << "\", " <<  size_hint << ");" << std::endl;
	return output_stream.str();
}

string replay_H5Gcreate2(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string loc_id = *tok_it;
	tok_it++;
	string name = *tok_it;
	tok_it++;
	string lcpl_id = *tok_it;
	tok_it++;
	string gcpl_id = *tok_it;
	tok_it++;
	string gapl_id = *tok_it;

	long loc_id_int = boost::lexical_cast<long>(loc_id);
	string loc_id_str;
	if (loc_id_int == 0) {
		loc_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(loc_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << loc_id << "\" loc id was not found in the map, ERROR!" << std::endl;
		} else {
			loc_id_str = (it->second);
		}
	}

	long lcpl_id_int = boost::lexical_cast<long>(lcpl_id);
	string lcpl_id_str;
	if (lcpl_id_int == 0) {
		lcpl_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(lcpl_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << lcpl_id << "\" lcpl id was not found in the map, ERROR!" << std::endl;
		} else {
			lcpl_id_str = (it->second);
		}
	}

	long gcpl_id_int = boost::lexical_cast<long>(gcpl_id);
	string gcpl_id_str;
	if (gcpl_id_int == 0) {
		gcpl_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(gcpl_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << gcpl_id << "\" gcpl id was not found in the map, ERROR!" << std::endl;
		} else {
			gcpl_id_str = (it->second);
		}
	}

	long gapl_id_int = boost::lexical_cast<long>(gapl_id);
	string gapl_id_str;
	if (gapl_id_int == 0) {
		gapl_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(gapl_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << gapl_id << "\" gapl id was not found in the map, ERROR!" << std::endl;
		} else {
			gapl_id_str = (it->second);
		}
	}

	long ret_id = boost::lexical_cast<long>(ret_value[0]);
	string ret_id_str;
	/*
	id_map_t::const_iterator it = id_map.find(ret_id);
	if(it == id_map.end()) {
		std::cerr << "\"" << ret_value[0] << "\" return id was not found in the map, adding it" << std::endl;
	*/
	ostringstream ret_id_stream;
	ret_id_stream << "hid_" << cur_id;
	cur_id++;
	ret_id_str = ret_id_stream.str();
	id_map.insert(std::pair<long, string>(ret_id, ret_id_str));
	/*
	}
	else {
		ret_id_str = (it->second);
	}
	*/

	ostringstream output_stream;
	output_stream << "hid_t " << ret_id_str << " = H5Gcreate2(" << loc_id_str << ", \"" << name << "\", " <<  lcpl_id_str << ", " << gcpl_id_str << ", " << gapl_id_str << ");" << std::endl;
	return output_stream.str();
}

string replay_H5Gget_objinfo(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {

}

string replay_H5Giterate(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {

}

string replay_H5Gopen1(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	if (ret_value[0] == "-1") {
		return "";
	}

	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string loc_id = *tok_it;
	tok_it++;
	string name = *tok_it;

	long loc_id_int = boost::lexical_cast<long>(loc_id);
	string loc_id_str;
	if (loc_id_int == 0) {
		loc_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(loc_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << loc_id << "\" loc id was not found in the map, ERROR!" << std::endl;
		} else {
			loc_id_str = (it->second);
		}
	}

	long ret_id = boost::lexical_cast<long>(ret_value[0]);
	string ret_id_str;
	/*
	id_map_t::const_iterator it = id_map.find(ret_id);
	if(it == id_map.end()) {
		std::cerr << "\"" << ret_value[0] << "\" return id was not found in the map, adding it" << std::endl;
	*/
	ostringstream ret_id_stream;
	ret_id_stream << "hid_" << cur_id;
	cur_id++;
	ret_id_str = ret_id_stream.str();
	id_map.insert(std::pair<long, string>(ret_id, ret_id_str));
	/*
	}
	else {
		ret_id_str = (it->second);
	}
	*/

	ostringstream output_stream;
	output_stream << "hid_t " << ret_id_str << " = H5Gopen1(" << loc_id_str << ", \"" << name << "\");" << std::endl;
	return output_stream.str();
}

string replay_H5Gopen2(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	if (ret_value[0] == "-1") {
		return "";
	}

	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string loc_id = *tok_it;
	tok_it++;
	string name = *tok_it;
	tok_it++;
	string gapl_id = *tok_it;

	long loc_id_int = boost::lexical_cast<long>(loc_id);
	string loc_id_str;
	if (loc_id_int == 0) {
		loc_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(loc_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << loc_id << "\" loc id was not found in the map, ERROR!" << std::endl;
		} else {
			loc_id_str = (it->second);
		}
	}

	long gapl_id_int = boost::lexical_cast<long>(gapl_id);
	string gapl_id_str;
	if (gapl_id_int == 0) {
		gapl_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(gapl_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << gapl_id << "\" gapl id was not found in the map, ERROR!" << std::endl;
		} else {
			gapl_id_str = (it->second);
		}
	}

	long ret_id = boost::lexical_cast<long>(ret_value[0]);
	string ret_id_str;
	/*
	id_map_t::const_iterator it = id_map.find(ret_id);
	if(it == id_map.end()) {
		std::cerr << "\"" << ret_value[0] << "\" return id was not found in the map, adding it" << std::endl;
	*/
	ostringstream ret_id_stream;
	ret_id_stream << "hid_" << cur_id;
	cur_id++;
	ret_id_str = ret_id_stream.str();
	id_map.insert(std::pair<long, string>(ret_id, ret_id_str));
	/*
	}
	else {
		ret_id_str = (it->second);
	}
	*/

	ostringstream output_stream;
	output_stream << "hid_t " << ret_id_str << " = H5Gopen2(" << loc_id_str << ", \"" << name << "\", " <<  gapl_id_str << ");" << std::endl;
	return output_stream.str();
}

// Dataset interface
string replay_H5Dclose(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	long dataset_id = boost::lexical_cast<long>(args[0]);

	string dataset_id_str;
	id_map_t::iterator it = id_map.find(dataset_id);
	if (it == id_map.end()) {
		std::cerr << "\"" << dataset_id << "\" dataset id was not found in the map, ERROR?!" << std::endl;
		dataset_id_str = args[0];
	} else {
		dataset_id_str = (it->second);

		if (same) {
			id_map.erase(it);
		}
	}

	ostringstream output_stream;
	output_stream << "H5Dclose(" << dataset_id_str << "); " << std::endl;

	string dummy_data_str;
	allocated_data_map_t::iterator it_alloc = allocated_data.find(dataset_id_str);
	if (it_alloc == allocated_data.end()) {
		std::cerr << "\"" << dataset_id_str << "\" dataset id was not found in the allocated datamap, not able to free!" << std::endl;
	} else {
		dummy_data_str = it_alloc->second;

		// TODO
#if 0
		allocated_rank_map_t::iterator it_rank = allocated_rank.find(dummy_data_str);
		if (it_rank == allocated_rank.end()) {
			output_stream << "free(" << dummy_data_str << ");" << std::endl;
		} else {
			int i;
			output_stream << "if(";
			vector<int> processors = allocated_rank[dummy_data_str];
			for (i = 0; i < processors.size() - 1; i++) {
				output_stream << "mpi_rank == " << processors[i] << " || ";
			}
			output_stream << "mpi_rank == " << processors[i];
			output_stream << ") {" << std::endl;
			output_stream << "free(" << dummy_data_str << ");" << std::endl;
			output_stream << "}" << std::endl;

			allocated_rank.erase(it_rank);
		}
#endif

		allocated_data.erase(it_alloc);
	}

	return output_stream.str();
}

string replay_H5Dcreate1(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string loc_id = *tok_it;
	tok_it++;
	string name = *tok_it;
	tok_it++;
	string dtype_id = *tok_it;
	tok_it++;
	string space_id = *tok_it;
	tok_it++;
	string dcpl_id = *tok_it;

	long loc_id_int = boost::lexical_cast<long>(loc_id);
	string loc_id_str;
	if (loc_id_int == 0) {
		loc_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(loc_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << loc_id << "\" loc id was not found in the map, ERROR!" << std::endl;
		} else {
			loc_id_str = (it->second);
		}
	}

	string dtype_id_str;
	try {
		long dtype_id_int = boost::lexical_cast<long>(dtype_id);
		if (dtype_id_int == 0) {
			dtype_id_str = "0";
		} else {
			id_map_t::const_iterator it = id_map.find(dtype_id_int);
			if (it == id_map.end()) {
				std::cerr << "\"" << dtype_id << "\" type id was not found in the map, ERROR!" << std::endl;
				dtype_id_str = dtype_id;
			} else {
				dtype_id_str = (it->second);
			}
		}
	} catch (boost::bad_lexical_cast &) {
		dtype_id_str = dtype_id;
	}

	long space_id_int = boost::lexical_cast<long>(space_id);
	string space_id_str;
	if (space_id_int == 0) {
		space_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(space_id_int);
		if (it == id_map.end()) {
			std::cerr << "H5Dcreate1: \"" << space_id << "\" space id was not found in the map, ERROR!" << std::endl;
		} else {
			space_id_str = (it->second);
		}
	}

	long dcpl_id_int = boost::lexical_cast<long>(dcpl_id);
	string dcpl_id_str;
	if (dcpl_id_int == 0) {
		dcpl_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(dcpl_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << dcpl_id << "\" dcpl id was not found in the map, ERROR!" << std::endl;
		} else {
			dcpl_id_str = (it->second);
		}
	}

	long ret_id = boost::lexical_cast<long>(ret_value[0]);
	string ret_id_str;
	/*
	id_map_t::const_iterator it = id_map.find(ret_id);
	if(it == id_map.end()) {
		std::cerr << "\"" << ret_value[0] << "\" return id was not found in the map, adding it" << std::endl;
	*/
	ostringstream ret_id_stream;
	ret_id_stream << "hid_" << cur_id;
	cur_id++;
	ret_id_str = ret_id_stream.str();
	id_map.insert(std::pair<long, string>(ret_id, ret_id_str));
	/*
	}
	else {
		ret_id_str = (it->second);
	}
	*/

	ostringstream output_stream;
	output_stream << "hid_t " << ret_id_str << " = H5Dcreate1(" << loc_id_str << ", \"" << name << "\", " <<  dtype_id_str << ", " << space_id_str << ", " << dcpl_id_str << ");" << std::endl;
	return output_stream.str();
}


string replay_H5Dcreate2(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string loc_id = *tok_it;
	tok_it++;
	string name = *tok_it;
	tok_it++;
	string dtype_id = *tok_it;
	tok_it++;
	string space_id = *tok_it;
	tok_it++;
	string lcpl_id = *tok_it;
	tok_it++;
	string dcpl_id = *tok_it;
	tok_it++;
	string dapl_id = *tok_it;

	long loc_id_int = boost::lexical_cast<long>(loc_id);
	string loc_id_str;
	if (loc_id_int == 0) {
		loc_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(loc_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << loc_id << "\" loc id was not found in the map, ERROR!" << std::endl;
		} else {
			loc_id_str = (it->second);
		}
	}

	string dtype_id_str;
	try {
		long dtype_id_int = boost::lexical_cast<long>(dtype_id);
		if (dtype_id_int == 0) {
			dtype_id_str = "0";
		} else {
			id_map_t::const_iterator it = id_map.find(dtype_id_int);
			if (it == id_map.end()) {
				std::cerr << "\"" << dtype_id << "\" type id was not found in the map, ERROR!" << std::endl;
				dtype_id_str = dtype_id;
			} else {
				dtype_id_str = (it->second);
			}
		}
	} catch (boost::bad_lexical_cast &) {
		dtype_id_str = dtype_id;
	}

	long space_id_int = boost::lexical_cast<long>(space_id);
	string space_id_str;
	if (space_id_int == 0) {
		space_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(space_id_int);
		if (it == id_map.end()) {
			std::cerr << "H5Dcreate2: \"" << space_id << "\" space id was not found in the map, ERROR!" << std::endl;
		} else {
			space_id_str = (it->second);
		}
	}

	long lcpl_id_int = boost::lexical_cast<long>(lcpl_id);
	string lcpl_id_str;
	if (lcpl_id_int == 0) {
		lcpl_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(lcpl_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << lcpl_id << "\" lcpl id was not found in the map, ERROR!" << std::endl;
		} else {
			lcpl_id_str = (it->second);
		}
	}

	long dcpl_id_int = boost::lexical_cast<long>(dcpl_id);
	string dcpl_id_str;
	if (dcpl_id_int == 0) {
		dcpl_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(dcpl_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << dcpl_id << "\" dcpl id was not found in the map, ERROR!" << std::endl;
		} else {
			dcpl_id_str = (it->second);
		}
	}

	long dapl_id_int = boost::lexical_cast<long>(dapl_id);
	string dapl_id_str;
	if (dapl_id_int == 0) {
		dapl_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(dapl_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << dapl_id << "\" dapl id was not found in the map, ERROR!" << std::endl;
		} else {
			dapl_id_str = (it->second);
		}
	}

	long ret_id = boost::lexical_cast<long>(ret_value[0]);
	string ret_id_str;
	/*
	id_map_t::const_iterator it = id_map.find(ret_id);
	if(it == id_map.end()) {
		std::cerr << "\"" << ret_value[0] << "\" return id was not found in the map, adding it" << std::endl;
	*/
	ostringstream ret_id_stream;
	ret_id_stream << "hid_" << cur_id;
	cur_id++;
	ret_id_str = ret_id_stream.str();
	id_map.insert(std::pair<long, string>(ret_id, ret_id_str));
	/*
	}
	else {
		ret_id_str = (it->second);
	}
	*/

	ostringstream output_stream;

	output_stream << "hid_t " << ret_id_str << " = H5Dcreate2(" << loc_id_str << ", \"" << name << "\", " <<  dtype_id_str << ", " << space_id_str << ", " << (lcpl_id_str == "0" ? "H5P_DEFAULT" : lcpl_id_str) << ", " << (dcpl_id_str == "0" ? "H5P_DEFAULT" : dcpl_id_str) << ", " << (dapl_id_str == "0" ? "H5P_DEFAULT" : dapl_id_str) << ");" << std::endl;

	return output_stream.str();
}

string replay_H5Dget_create_plist(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {

}

string replay_H5Dget_space(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	long dataset_id = boost::lexical_cast<long>(args[0]);

	string dataset_id_str;
	id_map_t::iterator it = id_map.find(dataset_id);
	if (it == id_map.end()) {
		std::cerr << "\"" << dataset_id << "\" dataset id was not found in the map, ERROR!" << std::endl;
	} else {
		dataset_id_str = (it->second);
	}

	long ret_id = boost::lexical_cast<long>(ret_value[0]);
	string ret_id_str;
	it = id_map.find(ret_id);
	if (it == id_map.end()) {
		std::cerr << "\"" << ret_value[0] << "\" return id was not found in the map, adding it" << std::endl;

		ostringstream ret_id_stream;
		ret_id_stream << "hid_" << cur_id;
		cur_id++;
		ret_id_str = ret_id_stream.str();
		id_map.insert(std::pair<long, string>(ret_id, ret_id_str));
	} else {
		ret_id_str = (it->second);
	}

	ostringstream output_stream;
	output_stream << "hid_t " << ret_id_str << " = " << "H5Dget_space(" << dataset_id_str << "); " << std::endl;
	return output_stream.str();
}

string replay_H5Dget_type(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {

}

string replay_H5Dopen1(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[])  {
	if (ret_value[0] == "-1") {
		return "";
	}

	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string loc_id = *tok_it;
	tok_it++;
	string name = *tok_it;

	long loc_id_int = boost::lexical_cast<long>(loc_id);
	string loc_id_str;
	if (loc_id_int == 0) {
		loc_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(loc_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << loc_id << "\" loc id was not found in the map, ERROR!" << std::endl;
		} else {
			loc_id_str = (it->second);
		}
	}

	long ret_id = boost::lexical_cast<long>(ret_value[0]);
	string ret_id_str;
	/*
	id_map_t::const_iterator it = id_map.find(ret_id);
	if(it == id_map.end()) {
		std::cerr << "\"" << ret_value[0] << "\" return id was not found in the map, adding it" << std::endl;
	*/
	ostringstream ret_id_stream;
	ret_id_stream << "hid_" << cur_id;
	cur_id++;
	ret_id_str = ret_id_stream.str();
	id_map.insert(std::pair<long, string>(ret_id, ret_id_str));
	/*
	}
	else {
		ret_id_str = (it->second);
	}
	*/

	ostringstream output_stream;
	output_stream << "hid_t " << ret_id_str << " = H5Dopen1(" << loc_id_str << ", \"" << name << ");" << std::endl;
	return output_stream.str();
}
string replay_H5Dopen2(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[])  {
	if (ret_value[0] == "-1") {
		return "";
	}

	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string loc_id = *tok_it;
	tok_it++;
	string name = *tok_it;
	tok_it++;
	string dapl_id = *tok_it;

	long loc_id_int = boost::lexical_cast<long>(loc_id);
	string loc_id_str;
	if (loc_id_int == 0) {
		loc_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(loc_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << loc_id << "\" loc id was not found in the map, ERROR!" << std::endl;
		} else {
			loc_id_str = (it->second);
		}
	}

	long dapl_id_int = boost::lexical_cast<long>(dapl_id);
	string dapl_id_str;
	if (dapl_id_int == 0) {
		dapl_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(dapl_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << dapl_id << "\" dapl id was not found in the map, ERROR!" << std::endl;
		} else {
			dapl_id_str = (it->second);
		}
	}

	long ret_id = boost::lexical_cast<long>(ret_value[0]);
	string ret_id_str;
	/*
	id_map_t::const_iterator it = id_map.find(ret_id);
	if(it == id_map.end()) {
		std::cerr << "\"" << ret_value[0] << "\" return id was not found in the map, adding it" << std::endl;
	*/
	ostringstream ret_id_stream;
	ret_id_stream << "hid_" << cur_id;
	cur_id++;
	ret_id_str = ret_id_stream.str();
	id_map.insert(std::pair<long, string>(ret_id, ret_id_str));
	/*
	}
	else {
		ret_id_str = (it->second);
	}
	*/

	//fprintf(outputfh,"\n\tH5Dopen2(%d,\"%s\",%d);\n", loc_id,name,dapl_id);
	ostringstream output_stream;
	output_stream << "hid_t " << ret_id_str << " = H5Dopen2(" << loc_id_str << ", \"" << name << "\", " <<  dapl_id_str << ");" << std::endl;
	return output_stream.str();
}


string replay_H5Dread(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string dataset_id = *tok_it;
	tok_it++;
	string mem_type_id = *tok_it;
	tok_it++;
	string mem_space_id = *tok_it;
	tok_it++;
	string file_space_id = *tok_it;
	tok_it++;
	string xfer_plist_id = *tok_it;

	long dataset_id_int = boost::lexical_cast<long>(dataset_id);
	string dataset_id_str;
	if (dataset_id_int == 0) {
		dataset_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(dataset_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << dataset_id << "\" loc id was not found in the map, ERROR!" << std::endl;
		} else {
			dataset_id_str = (it->second);
		}
	}

	string mem_type_id_str;
	try {
		long mem_type_id_int = boost::lexical_cast<long>(mem_type_id);
		if (mem_type_id_int == 0) {
			mem_type_id_str = "0";
		} else {
			id_map_t::const_iterator it = id_map.find(mem_type_id_int);
			if (it == id_map.end()) {
				std::cerr << "\"" << mem_type_id << "\" dtype id was not found in the map, USING THE NUMBER!" << std::endl;
				mem_type_id_str = mem_type_id;
			} else {
				mem_type_id_str = (it->second);
			}
		}
	} catch (boost::bad_lexical_cast &) {
		mem_type_id_str = mem_type_id;
	}

	long mem_space_id_int = boost::lexical_cast<long>(mem_space_id);
	string mem_space_id_str;
	if (mem_space_id_int == 0) {
		mem_space_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(mem_space_id_int);
		if (it == id_map.end()) {
			std::cerr << "H5Dread: \"" << mem_space_id << "\" space id was not found in the map, ERROR!" << std::endl;
		} else {
			mem_space_id_str = (it->second);
		}
	}

	long file_space_id_int = boost::lexical_cast<long>(file_space_id);
	string file_space_id_str;
	if (file_space_id_int == 0) {
		file_space_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(file_space_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << file_space_id << "\" lcpl id was not found in the map, ERROR!" << std::endl;
		} else {
			file_space_id_str = (it->second);
		}
	}

	long xfer_plist_id_int = boost::lexical_cast<long>(xfer_plist_id);
	string xfer_plist_id_str;
	if (xfer_plist_id_int == 0) {
		xfer_plist_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(xfer_plist_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << xfer_plist_id << "\" xfer_plist id was not found in the map, ERROR!" << std::endl;
		} else {
			xfer_plist_id_str = (it->second);
		}
	}

	ostringstream output_stream;

	string its_args[1], its_ret_value[1];
	its_args[0] = mem_space_id;
	output_stream << replay_H5Sget_select_npoints(ranks, same, rank_ids, diffArgs, loop_size, its_args, its_ret_value);
	string npoints_str = its_ret_value[0];

	its_args[0] = mem_type_id_str;
	output_stream << replay_H5Tget_size(ranks, same, rank_ids, diffArgs, loop_size, its_args, its_ret_value);
	string size_dtype_str = its_ret_value[0];

	string total_size_str;
	ostringstream total_size_id_stream;
	total_size_id_stream << "total_size_" << data_id;
	data_id++;
	total_size_str = total_size_id_stream.str();
	output_stream << "long long " << total_size_str << " = " << npoints_str  << " * " << size_dtype_str << ";" << std::endl;

	string dummy_data_str;
	ostringstream dummy_data_id_stream;
	dummy_data_id_stream << "dummy_data_" << data_id;
	data_id++;
	dummy_data_str = dummy_data_id_stream.str();

	output_stream << "void *" << dummy_data_str << " = (void *) malloc(" << total_size_str << ");" << std::endl;
	allocated_data[dataset_id_str] = dummy_data_str;
	if (!same) {
		vector<int> processors(rank_ids, rank_ids + ranks);
		allocated_rank[dummy_data_str] = processors;
	}


	output_stream << "H5Dread(" << dataset_id_str << ", " <<  mem_type_id_str << ", " << mem_space_id_str << ", " << file_space_id_str << ", " << xfer_plist_id_str << ", " << dummy_data_str << ");" << std::endl;

	output_stream << "free(" << dummy_data_str << ");" << std::endl;

	return output_stream.str();
}

string replay_H5Dwrite(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string dataset_id = *tok_it;
	tok_it++;
	string mem_type_id = *tok_it;
	tok_it++;
	string mem_space_id = *tok_it;
	tok_it++;
	string file_space_id = *tok_it;
	tok_it++;
	string xfer_plist_id = *tok_it;

	long dataset_id_int = boost::lexical_cast<long>(dataset_id);
	string dataset_id_str;
	if (dataset_id_int == 0) {
		dataset_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(dataset_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << dataset_id << "\" loc id was not found in the map, ERROR!" << std::endl;
		} else {
			dataset_id_str = (it->second);
		}
	}

	string mem_type_id_str;
	try {
		long mem_type_id_int = boost::lexical_cast<long>(mem_type_id);
		if (mem_type_id_int == 0) {
			mem_type_id_str = "0";
		} else {
			id_map_t::const_iterator it = id_map.find(mem_type_id_int);
			if (it == id_map.end()) {
				std::cerr << "\"" << mem_type_id << "\" dtype id was not found in the map, USING THE NUMBER!" << std::endl;
				mem_type_id_str = mem_type_id;
			} else {
				mem_type_id_str = (it->second);
			}
		}
	} catch (boost::bad_lexical_cast &) {
		mem_type_id_str = mem_type_id;
	}

	long mem_space_id_int = boost::lexical_cast<long>(mem_space_id);
	string mem_space_id_str;
	if (mem_space_id_int == 0) {
		mem_space_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(mem_space_id_int);
		if (it == id_map.end()) {
			std::cerr << "H5Dwrite: \"" << mem_space_id << "\" space id was not found in the map, ERROR!" << std::endl;
		} else {
			mem_space_id_str = (it->second);
		}
	}

	long file_space_id_int = boost::lexical_cast<long>(file_space_id);
	string file_space_id_str;
	if (file_space_id_int == 0) {
		file_space_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(file_space_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << file_space_id << "\" lcpl id was not found in the map, ERROR!" << std::endl;
		} else {
			file_space_id_str = (it->second);
		}
	}

	long xfer_plist_id_int = boost::lexical_cast<long>(xfer_plist_id);
	string xfer_plist_id_str;
	if (xfer_plist_id_int == 0) {
		xfer_plist_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(xfer_plist_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << xfer_plist_id << "\" xfer_plist id was not found in the map, ERROR!" << std::endl;
		} else {
			xfer_plist_id_str = (it->second);
		}
	}

	ostringstream output_stream;

	string its_args[1], its_ret_value[1];
	its_args[0] = mem_space_id;
	output_stream << replay_H5Sget_select_npoints(ranks, same, rank_ids, diffArgs, loop_size, its_args, its_ret_value);
	string npoints_str = its_ret_value[0];

	its_args[0] = mem_type_id_str;
	output_stream << replay_H5Tget_size(ranks, same, rank_ids, diffArgs, loop_size, its_args, its_ret_value);
	string size_dtype_str = its_ret_value[0];

	string total_size_str;
	ostringstream total_size_id_stream;
	total_size_id_stream << "total_size_" << data_id;
	data_id++;
	total_size_str = total_size_id_stream.str();
	output_stream << "long long " << total_size_str << " = " << npoints_str  << " * " << size_dtype_str << ";" << std::endl;

	string dummy_data_str;
	ostringstream dummy_data_id_stream;
	dummy_data_id_stream << "dummy_data_" << data_id;
	data_id++;
	dummy_data_str = dummy_data_id_stream.str();

	output_stream << "void *" << dummy_data_str << " = (void *) malloc(" << total_size_str << ");" << std::endl;
	allocated_data[dataset_id_str] = dummy_data_str;
	if (!same) {
		vector<int> processors(rank_ids, rank_ids + ranks);
		allocated_rank[dummy_data_str] = processors;
	}

	output_stream << "H5Dwrite(" << dataset_id_str << ", " <<  mem_type_id_str << ", " << mem_space_id_str << ", " << file_space_id_str << ", " << xfer_plist_id_str << ", " << dummy_data_str << ");" << std::endl;

	output_stream << "free(" << dummy_data_str << ");" << std::endl;

	return output_stream.str();
}

string replay_H5Sclose(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	long space_id = boost::lexical_cast<long>(args[0]);

	string space_id_str;
	id_map_t::iterator it = id_map.find(space_id);
	if (it == id_map.end()) {
		std::cerr << "H5Sclose: \"" << space_id << "\" space id was not found in the map, ERROR!" << std::endl;
	} else {
		space_id_str = (it->second);

		if (same) {
			id_map.erase(it);
		}
	}

	//fprintf(outputfh,"\n\tH5Sclose(%d);\n",space_id);
	ostringstream output_stream;
	output_stream << "H5Sclose(" << space_id_str << "); " << std::endl;
	return output_stream.str();
}

string replay_H5Screate(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	long ret_id = boost::lexical_cast<long>(ret_value[0]);

	string ret_id_str;
	/*
	id_map_t::const_iterator it = id_map.find(ret_id);
	if(it == id_map.end()) {
		std::cerr << "\"" << ret_value[0] << "\" return id was not found in the map, adding it" << std::endl;
	*/
	ostringstream ret_id_stream;
	ret_id_stream << "hid_" << cur_id;
	cur_id++;
	ret_id_str = ret_id_stream.str();
	id_map.insert(std::pair<long, string>(ret_id, ret_id_str));
	/*
	}
	else {
		ret_id_str = (it->second);
	}
	*/

	ostringstream output_stream;
	output_stream << "hid_t " << ret_id_str << " = H5Screate(" << args[0] << "); " << std::endl;
	return output_stream.str();
}

string replay_H5Screate_simple(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	// parse the args
	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string rank = *tok_it;
	tok_it++;
	string current_dims = *tok_it;
	replace(current_dims.begin(), current_dims.end(), ';', ',');
	tok_it++;
	string maximum_dims = *tok_it;
	replace(maximum_dims.begin(), maximum_dims.end(), ';', ',');
	if (loop_size) {
		replace(current_dims.begin(), current_dims.end(), '<', '{');
		replace(current_dims.begin(), current_dims.end(), '>', '}');
		replace(maximum_dims.begin(), maximum_dims.end(), '<', '{');
		replace(maximum_dims.begin(), maximum_dims.end(), '>', '}');
	}

	ostringstream output_stream;

	string cur_dims_id_str;
	ostringstream cur_dims_id_stream;
	cur_dims_id_stream << "cur_dims_" << dim_id;
	dim_id++;
	cur_dims_id_str = cur_dims_id_stream.str();
	if (loop_size) {
		output_stream << "hsize_t " << cur_dims_id_str << "[" << loop_size << "][" << rank << "] = " << current_dims << ";" << std::endl;
	} else {
		output_stream << "hsize_t " << cur_dims_id_str << "[] = " << current_dims << ";" << std::endl;
	}

	string max_dims_id_str;
	if (maximum_dims != "NULL") {
		ostringstream max_dims_id_stream;
		max_dims_id_stream << "max_dims_" << dim_id;
		dim_id++;
		max_dims_id_str = max_dims_id_stream.str();

		if (loop_size) {
			output_stream << "hsize_t " << max_dims_id_str << "[" << loop_size << "][" << rank << "] = " << maximum_dims << ";" << std::endl;
		} else {
			output_stream << "hsize_t " << max_dims_id_str << "[] = " << maximum_dims << ";" << std::endl;
		}
	}

	long ret_id = boost::lexical_cast<long>(ret_value[0]);

	string ret_id_str;
	ostringstream ret_id_stream;
	ret_id_stream << "hid_" << cur_id;
	cur_id++;
	ret_id_str = ret_id_stream.str();
	id_map.insert(std::pair<long, string>(ret_id, ret_id_str));

	if (loop_size) {
		if (maximum_dims == "NULL") {
			output_stream << "hid_t " << ret_id_str << " = H5Screate_simple(" << rank << ", " << cur_dims_id_str << "[loop_index], NULL);" << std::endl;
		} else {
			output_stream << "hid_t " << ret_id_str << " = H5Screate_simple(" << rank << ", " << cur_dims_id_str << "[loop_index], " << max_dims_id_str << "[loop_index]);" << std::endl;
		}
	} else {
		if (maximum_dims == "NULL") {
			output_stream << "hid_t " << ret_id_str << " = H5Screate_simple(" << rank << ", " << cur_dims_id_str << ", NULL);" << std::endl;
		} else {
			output_stream << "hid_t " << ret_id_str << " = H5Screate_simple(" << rank << ", " << cur_dims_id_str << ", " << max_dims_id_str << ");" << std::endl;
		}
	}

	return output_stream.str();
}

string replay_H5Sget_select_npoints(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[])  {
	long space_id = boost::lexical_cast<long>(args[0]);

	string space_id_str;
	id_map_t::iterator it = id_map.find(space_id);
	if (it == id_map.end()) {
		std::cerr << "H5Sget_select_npoints: \"" << space_id << "\" space id was not found in the map, ERROR!" << std::endl;
	} else {
		space_id_str = (it->second);
	}

	string ret_id_str;
	ostringstream ret_id_stream;
	ret_id_stream << "npoints_" << dim_id;
	dim_id++;
	ret_id_str = ret_id_stream.str();

	ostringstream output_stream;
	output_stream << "hssize_t " << ret_id_str << " = H5Sget_select_npoints(" << space_id_str << ");" << std::endl;

	ret_value[0] = ret_id_str;

	return output_stream.str();
}

string replay_H5Sget_simple_extent_dims(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}
string replay_H5Sget_simple_extent_npoints(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}
string replay_H5Sselect_elements(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

bool check_for_different_arrays(vector<string> str) {
	for (int i = 1; i < str.size(); i++) {
		if (str[i] != str[i - 1]) {
			return true;
		}
	}

	return false;
}

string smart_generate_different_arrays(string id_str, vector<vector< vector<int64_t> > > arr, int loop_size, string &ret_result) {
	int nranks = arr.size();
	int nloops = arr[0].size();
	int ndims = arr[0][0].size();
	cout << "nranks = " << nranks << ", nloops = " << nloops << ", ndims = " << ndims << endl;

	if (loop_size == 0) {
		// Looking for a linear formula y = ax + b
		cout << "Looking for linear relationship in " << id_str << "[]" << endl;
		string ret_value = "hsize_t " + id_str + "[" + boost::lexical_cast<std::string>(ndims) + "];\n";
		for (int dim = 0; dim < ndims; dim++) {
			double b = arr[0][0][dim];
			double a = arr[1][0][dim] - arr[0][0][dim];
			// cout << "a = " << a << ", b = " << b << endl;
			int prev_rank = 1;
			for (int i = 2; i < nranks; i++) {
				// cout << arr[i][0][dim] - arr[prev_rank][0][dim] << endl;
				if (arr[i][0][dim] - arr[prev_rank][0][dim] != a) {
					ret_result = "NO";
					return "";
				}
				prev_rank = i;
			}

			ret_value += (id_str + "[" + boost::lexical_cast<std::string>(dim) + "] = (hsize_t) " + boost::lexical_cast<std::string>(a) + " * mpi_rank + " + boost::lexical_cast<std::string>(b) + ";\n");
		}

		ret_result = "YES";
		return ret_value;
	}
}

string smart_generate_different_loops(string id_str, vector<vector< vector<int64_t> > > arr, int loop_size, string &ret_result) {
	int nranks = arr.size();
	int ndims = arr[0].size();
	int nloops = arr[0][0].size();
	int nDegree = 3;
	cout << "nranks = " << nranks << ", ndims = " << ndims << ", nloops = " << arr[0][0].size() << endl;

	for (int i = 0; i < nranks; i++) {
		for (int j = 0; j < ndims; j++) {
			vector<long> X;
			for (long k = 0; k < nloops; k++) {
				X.push_back(k);
			}

			boost::numeric::ublas::matrix<long> oXMatrix(nloops, nDegree);
			boost::numeric::ublas::matrix<long> oYMatrix(nloops, 1);

			for (size_t k = 0; k < nloops; k++) {
				oYMatrix(k, 0) = arr[i][j][k];
			}

			for (size_t nRow = 0; nRow < nloops; nRow++) {
				long nVal = 1;
				for (int nCol = 0; nCol < nDegree; nCol++) {
					oXMatrix(nRow, nCol) = nVal;
					nVal *= X[nRow];
				}
			}
			cout << "Set the data, going to transpose..." << endl;

			// transpose X matrix
			boost::numeric::ublas::matrix<long> oXtMatrix(trans(oXMatrix));
			// multiply transposed X matrix with X matrix
			boost::numeric::ublas::matrix<long> oXtXMatrix(prec_prod(oXtMatrix, oXMatrix));
			// multiply transposed X matrix with Y matrix
			boost::numeric::ublas::matrix<long> oXtYMatrix(prec_prod(oXtMatrix, oYMatrix));

			cout << "Transposed the data, going to decompose..." << endl;

			try {
				// lu decomposition
				boost::numeric::ublas::permutation_matrix<int> pert(oXtXMatrix.size1());
				const std::size_t singular = lu_factorize(oXtXMatrix, pert);
				// must be singular
				// BOOST_ASSERT(singular == 0);

				cout << "Decomposed the data, going to backsubstitue..." << endl;
				// backsubstitution
				boost::numeric::ublas::lu_substitute(oXtXMatrix, pert, oXtYMatrix);
			} catch (...) {
				return "NO";
			}

			// copy the result to coeff
			vector<double> coeff = vector<double>(oXtYMatrix.data().begin(), oXtYMatrix.data().end());
			cout << "Found coeff of size " << coeff.size() << endl;

			return "YES";
		}
	}
}

string generate_different_arrays(string arr_name, vector< vector< vector<int64_t> > > arr, vector<string> str, int loop_size, string &ret_id) {
	int nranks = arr.size();
	int ndims = arr[0].size();

	string id_str;
	ostringstream id_stream;
	id_stream << arr_name << "_" << dim_id;
	id_str = id_stream.str();

	ostringstream output_stream;
	string ret_result;

	string ret_value = smart_generate_different_arrays(id_str, arr, loop_size, ret_result);

	if (ret_result != "NO") {
		cout << ret_value << endl;
		output_stream << ret_value;
	} else {
		output_stream << "hsize_t " << id_str << "[" << nranks << "][" << ndims << "] = {";
		int i;
		for (i = 0; i < str.size() - 1; i++) {
			output_stream << str[i] << ",";
		}
		output_stream << str[i];
		output_stream << "};" << std::endl;
		id_str.append("[mpi_rank]");
	}

	ret_id = id_str;

	return output_stream.str();
}

string replay_H5Sselect_hyperslab(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	ostringstream output_stream;

	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string space_id_arg = *tok_it;
	tok_it++;
	string op = *tok_it;

	long space_id = boost::lexical_cast<long>(space_id_arg);
	string space_id_str;
	id_map_t::const_iterator it = id_map.find(space_id);
	if (it == id_map.end()) {
		std::cerr << "H5Sselect_hyperslab: \"" << space_id << "\" space id was not found in the map, error!" << std::endl;
	} else {
		space_id_str = (it->second);
	}

	// cout << "same = " << same << ", and ranks = " << ranks << endl;
	vector< vector<vector<int64_t> > > starts_int, strides_int, counts_int, blocks_int;
	vector<string> starts_str, strides_str, counts_str, blocks_str;
	bool starts_null = true, strides_null = true, counts_null = true, blocks_null = true;

	int ndims = 0;
	int n_iter = ranks;
	if (diffArgs == 0) {
		n_iter = 1;
	}

	for (int i = 0; i < n_iter; i++) {
		ndims = 0;
		boost::char_separator<char> sep(",");
		boost::char_separator<char> semicolon(";");
		boost::char_separator<char> seps(",{}<>");
		boost::tokenizer< boost::char_separator<char> > tok(args[i], sep);
		boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
		string space_id_arg = *tok_it;

		tok_it++;
		string op = *tok_it;

		tok_it++;
		string start = *tok_it;
		if (start != "NULL") {
			starts_null = false;
			vector<int64_t> starts_of_this_dim;
			vector<vector<int64_t> > starts_of_this_loop;
			boost::tokenizer< boost::char_separator<char> > tok_start(start, seps);
			boost::tokenizer< boost::char_separator<char> >::iterator tok_start_it = tok_start.begin();

			for (tok_start_it = tok_start.begin(); tok_start_it != tok_start.end(); ++tok_start_it) {
				boost::tokenizer< boost::char_separator<char> > tok_start_dim(*tok_start_it, semicolon);
				boost::tokenizer< boost::char_separator<char> >::iterator tok_start_dim_it = tok_start_dim.begin();
				for(tok_start_dim_it = tok_start_dim.begin(); tok_start_dim_it != tok_start_dim.end(); ++tok_start_dim_it) {
					starts_of_this_dim.push_back(boost::lexical_cast<int64_t>(*tok_start_dim_it));
					ndims++;
				}
				starts_of_this_loop.push_back(starts_of_this_dim);

				for (int i = 1; i < loop_size; i++) {
					++tok_start_it;
					starts_of_this_dim.clear();
					boost::tokenizer< boost::char_separator<char> > tok_start_dim(*tok_start_it, semicolon);
					boost::tokenizer< boost::char_separator<char> >::iterator tok_start_dim_it = tok_start_dim.begin();
					for(tok_start_dim_it = tok_start_dim.begin(); tok_start_dim_it != tok_start_dim.end(); ++tok_start_dim_it) {
						starts_of_this_dim.push_back(boost::lexical_cast<int64_t>(*tok_start_dim_it));
					}
					starts_of_this_loop.push_back(starts_of_this_dim);
				}

			}
			starts_int.push_back(starts_of_this_loop);
		}
		replace(start.begin(), start.end(), ';', ',');
		starts_str.push_back(start);

		tok_it++;
		string stride = *tok_it;
		replace(stride.begin(), stride.end(), ';', ',');
		strides_str.push_back(stride);
		if (stride != "NULL") {
			strides_null = false;
			vector<int64_t> strides_of_this_loop;
			vector<vector<int64_t> > strides_of_this_rank;
			boost::tokenizer< boost::char_separator<char> > tok_stride(stride, seps);
			boost::tokenizer< boost::char_separator<char> >::iterator tok_stride_it = tok_stride.begin();

			for (tok_stride_it = tok_stride.begin(); tok_stride_it != tok_stride.end(); ++tok_stride_it) {
				strides_of_this_loop.push_back(boost::lexical_cast<int64_t>(*tok_stride_it));
				for (int i = 1; i < loop_size; i++) {
					++tok_stride_it;
					strides_of_this_loop.push_back(boost::lexical_cast<int64_t>(*tok_stride_it));
				}
				strides_of_this_rank.push_back(strides_of_this_loop);
			}
			strides_int.push_back(strides_of_this_rank);
		}

		tok_it++;
		string count = *tok_it;
		replace(count.begin(), count.end(), ';', ',');
		counts_str.push_back(count);
		if (count != "NULL") {
			counts_null = false;
			vector<int64_t> counts_of_this_loop;
			vector<vector<int64_t> > counts_of_this_rank;
			boost::tokenizer< boost::char_separator<char> > tok_count(count, seps);
			boost::tokenizer< boost::char_separator<char> >::iterator tok_count_it = tok_count.begin();

			for (tok_count_it = tok_count.begin(); tok_count_it != tok_count.end(); ++tok_count_it) {
				counts_of_this_loop.push_back(boost::lexical_cast<int64_t>(*tok_count_it));
				for (int i = 1; i < loop_size; i++) {
					++tok_count_it;
					counts_of_this_loop.push_back(boost::lexical_cast<int64_t>(*tok_count_it));
				}
				counts_of_this_rank.push_back(counts_of_this_loop);
			}
			counts_int.push_back(counts_of_this_rank);
		}



		tok_it++;
		string block = *tok_it;
		replace(block.begin(), block.end(), ';', ',');
		blocks_str.push_back(block);
		if (block != "NULL") {
			blocks_null = false;
			vector<int64_t> blocks_of_this_loop;
			vector<vector<int64_t> > blocks_of_this_rank;
			boost::tokenizer< boost::char_separator<char> > tok_block(block, seps);
			boost::tokenizer< boost::char_separator<char> >::iterator tok_block_it = tok_block.begin();
			for (tok_block_it = tok_block.begin(); tok_block_it != tok_block.end(); ++tok_block_it) {
				blocks_of_this_loop.push_back(boost::lexical_cast<int64_t>(*tok_block_it));
				for (int i = 1; i < loop_size; i++) {
					++tok_block_it;
					blocks_of_this_loop.push_back(boost::lexical_cast<int64_t>(*tok_block_it));
				}
				blocks_of_this_rank.push_back(blocks_of_this_loop);
			}
			blocks_int.push_back(blocks_of_this_rank);
		}
	}

	string start_id_str, stride_id_str, count_id_str, block_id_str;
	ostringstream start_id_stream, stride_id_stream, count_id_stream, block_id_stream;

	int nranks = starts_str.size();
	/*if (ndims > 1) {
		cerr << "ndims = " << ndims << " - ERROR: CURRENTLY NOT IMPLEMENTED! IT's a TODO" << endl;
	}*/

	if (!starts_null) {
		string ret_id;

		if (loop_size == 0) {
			if (check_for_different_arrays(starts_str)) {
				output_stream << generate_different_arrays("start", starts_int, starts_str, loop_size, ret_id);
				start_id_str = ret_id;
			} else {
				start_id_stream << "start_" << dim_id;
				start_id_str = start_id_stream.str();
				output_stream << "hsize_t " << start_id_str << "[] =  " << starts_str[0] << ";" << std::endl;
			}
		} else {
			if (smart_generate_different_loops("start", starts_int, loop_size, ret_id) != "NO") {
				cout << "YES..." << endl;
			} else {
				start_id_stream << "start_" << dim_id;
				start_id_str = start_id_stream.str();
				output_stream << "hsize_t " << start_id_str << "[" << nranks << "][" << loop_size << "][" << ndims << "] = { " << std::endl;
				int i;
				for (i = 0; i < starts_str.size() - 1; i++) {
					replace(starts_str[i].begin(), starts_str[i].end(), '<', '{');
					replace(starts_str[i].begin(), starts_str[i].end(), '>', '}');
					output_stream << starts_str[i] << "," << std::endl;
				}

				replace(starts_str[i].begin(), starts_str[i].end(), '<', '{');
				replace(starts_str[i].begin(), starts_str[i].end(), '>', '}');
				output_stream << starts_str[i] << std::endl;
				output_stream << "};" << std::endl;
				start_id_str.append("[mpi_rank][loop_index]");
			}
		}
	}

	if (!strides_null) {
		if (check_for_different_arrays(strides_str)) {
			string ret_id;
			output_stream << generate_different_arrays("stride", strides_int, strides_str, loop_size, ret_id);
			stride_id_str = ret_id;
		} else {
			stride_id_stream << "stride_" << dim_id;
			stride_id_str = stride_id_stream.str();
			if (loop_size == 0) {
				output_stream << "hsize_t " << stride_id_str << "[] =  " << strides_str[0] << ";" << std::endl;
			} else {
				// TODO: Just copy from start/count
			}
		}
	}

	if (!counts_null) {
		string ret_id;
		if (loop_size == 0) {
			if (check_for_different_arrays(counts_str)) {
				output_stream << generate_different_arrays("count", counts_int, counts_str, loop_size, ret_id);
				count_id_str = ret_id;
			} else {
				count_id_stream << "count_" << dim_id;
				count_id_str = count_id_stream.str();
				output_stream << "hsize_t " << count_id_str << "[] =  " << counts_str[0] << ";" << std::endl;
			}
		} else {
			if (smart_generate_different_loops("count", counts_int, loop_size, ret_id) != "NO") {
			} else {
				count_id_stream << "count_" << dim_id;
				count_id_str = count_id_stream.str();
				output_stream << "hsize_t " << count_id_str << "[" << nranks << "][" << loop_size << "][" << ndims << "] = { " << std::endl;
				int i;
				for (i = 0; i < counts_str.size() - 1; i++) {
					replace(counts_str[i].begin(), counts_str[i].end(), '<', '{');
					replace(counts_str[i].begin(), counts_str[i].end(), '>', '}');
					output_stream << counts_str[i] << "," << std::endl;
				}
				replace(counts_str[i].begin(), counts_str[i].end(), '<', '{');
				replace(counts_str[i].begin(), counts_str[i].end(), '>', '}');
				output_stream << counts_str[i] << std::endl;
				output_stream << "};" << std::endl;
				count_id_str.append("[mpi_rank][loop_index]");
			}
		}
	}

	if (!blocks_null) {
		if (check_for_different_arrays(blocks_str)) {
			string ret_id;
			output_stream << generate_different_arrays("block", blocks_int, blocks_str, loop_size, ret_id);
			block_id_str = ret_id;
		} else {
			block_id_stream << "block_" << dim_id;
			block_id_str = block_id_stream.str();
			if (loop_size == 0) {
				output_stream << "hsize_t " << block_id_str << "[] =  " << blocks_str[0] << ";" << std::endl;
			} else {
				// TODO: Just copy from start/count
			}
		}
	}

	output_stream << "H5Sselect_hyperslab(" << space_id_str << ", " << op << ", " << (starts_null ? "NULL" : start_id_str) << ", " << (strides_null ? "NULL" : stride_id_str) << ", " << (counts_null ? "NULL" : count_id_str) << ", " << (blocks_null ? "NULL" : block_id_str) << ");" << std::endl;

	dim_id++;

	return output_stream.str();
}

string replay_H5Sselect_none(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Tclose(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	long dtype_id = boost::lexical_cast<long>(args[0]);

	string dtype_id_str;
	id_map_t::iterator it = id_map.find(dtype_id);
	if (it == id_map.end()) {
		std::cerr << "H5Tclose: \"" << dtype_id << "\" type id was not found in the map, ERROR!" << std::endl;
	} else {
		dtype_id_str = (it->second);

		if (same) {
			id_map.erase(it);
		}
	}

	ostringstream output_stream;
	output_stream << "H5Tclose(" << dtype_id_str << "); " << std::endl;
	return output_stream.str();
}

string replay_H5Tcreate(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string class_id = *tok_it;
	tok_it++;
	string size = *tok_it;
	tok_it++;

	long ret_id = boost::lexical_cast<long>(ret_value[0]);

	string ret_id_str;

	/*
	id_map_t::const_iterator it = id_map.find(ret_id);
	if(it == id_map.end()) {
		std::cerr << "\"" << ret_value[0] << "\" return id was not found in the map, adding it" << std::endl;
	*/
	ostringstream ret_id_stream;
	ret_id_stream << "hid_" << cur_id;
	cur_id++;
	ret_id_str = ret_id_stream.str();
	id_map.insert(std::pair<long, string>(ret_id, ret_id_str));
	/*
	}
	else {
		ret_id_str = (it->second);
	}
	*/

	ostringstream output_stream;
	output_stream << "hid_t " << ret_id_str << " = H5Tcreate(" << class_id << ", " << size << "); " << std::endl;
	return output_stream.str();
}

string replay_H5Topen1(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Topen2(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Tcommit1(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Tcommit2(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Tinsert(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	// ( hid_t dtype_id, const char * name, size_t offset, hid_t field_id )
	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string dtype_id_str = *tok_it;
	tok_it++;
	string name = *tok_it;
	tok_it++;
	string offset = *tok_it;
	tok_it++;
	string field_id = *tok_it;

	long dtype_id = boost::lexical_cast<long>(dtype_id_str);
	id_map_t::iterator it = id_map.find(dtype_id);
	if (it == id_map.end()) {
		std::cerr << "H5Tinsert: \"" << dtype_id << "\" type id was not found in the map, ERROR!" << std::endl;
	} else {
		dtype_id_str = (it->second);
	}


	ostringstream output_stream;
	output_stream << "H5Tinsert(" << dtype_id_str << ", \"" << name << "\", " << offset << ", " << field_id << "); " << std::endl;
	return output_stream.str();
}

string replay_H5Tcopy(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	long ret_id = boost::lexical_cast<long>(ret_value[0]);

	string ret_id_str;

	/*
	id_map_t::const_iterator it = id_map.find(ret_id);
	if(it == id_map.end()) {
		std::cerr << "\"" << ret_value[0] << "\" return id was not found in the map, adding it" << std::endl;
	*/
	ostringstream ret_id_stream;
	ret_id_stream << "hid_" << cur_id;
	cur_id++;
	ret_id_str = ret_id_stream.str();
	id_map.insert(std::pair<long, string>(ret_id, ret_id_str));
	/*
	}
	else {
		ret_id_str = (it->second);
	}
	*/

	ostringstream output_stream;
	output_stream << "hid_t " << ret_id_str << " = H5Tcopy(" << args[0] << "); " << std::endl;
	return output_stream.str();
}

string replay_H5Tget_class(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Tget_size(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	string dtype_id_str = args[0];

	string ret_id_str;
	ostringstream ret_id_stream;
	ret_id_stream << "size_dtype_" << dim_id;
	dim_id++;
	ret_id_str = ret_id_stream.str();

	ostringstream output_stream;
	output_stream << "size_t " << ret_id_str << " = H5Tget_size(" << dtype_id_str << ");" << std::endl;
	ret_value[0] = ret_id_str;

	return output_stream.str();
}

string replay_H5Tset_size(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string dtype_id = *tok_it;
	tok_it++;
	string size = *tok_it;

	string dtype_id_name;
	id_map_t::const_iterator it = id_map.find(boost::lexical_cast<int>(dtype_id));
	if (it == id_map.end()) {
		std::cerr << "\"" << ret_value[0] << "\" not found in the map! ERROR!" << std::endl;
	} else {
		dtype_id_name = (it->second);
	}


	ostringstream output_stream;
	output_stream << "H5Tset_size(" << dtype_id_name << ", " << size << ");" << std::endl;
	return output_stream.str();

}

string replay_H5Aclose(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	long attr_id = boost::lexical_cast<long>(args[0]);

	string attr_id_str;
	id_map_t::iterator it = id_map.find(attr_id);
	if (it == id_map.end()) {
		std::cerr << "\"" << attr_id << "\" attr id was not found in the map, ERROR!" << std::endl;
	} else {
		attr_id_str = (it->second);

		if (same) {
			id_map.erase(it);
		}
	}

	ostringstream output_stream;

	output_stream << "H5Aclose(" << attr_id_str << "); " << std::endl;

	string dummy_data_str;
	allocated_data_map_t::iterator it_alloc = allocated_data.find(attr_id_str);
	if (it_alloc == allocated_data.end()) {
		std::cerr << "\"" << attr_id_str << "\" attr id was not found in the allocated datamap, not able to free!" << std::endl;
	} else {
		// TODO
#if 0
		dummy_data_str = it_alloc->second;

		allocated_rank_map_t::iterator it_rank = allocated_rank.find(dummy_data_str);
		if (it_rank == allocated_rank.end()) {
			output_stream << "free(" << dummy_data_str << ");" << std::endl;
		} else {
			int i;
			output_stream << "if(";
			vector<int> processors = allocated_rank[dummy_data_str];
			for (i = 0; i < processors.size() - 1; i++) {
				output_stream << "mpi_rank == " << processors[i] << " || ";
			}
			output_stream << "mpi_rank == " << processors[i];
			output_stream << ") {" << std::endl;
			output_stream << "free(" << dummy_data_str << ");" << std::endl;
			output_stream << "}" << std::endl;

			allocated_rank.erase(it_rank);
		}
#endif

		allocated_data.erase(it_alloc);
	}

	return output_stream.str();
}

string replay_H5Acreate1(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string loc_id = *tok_it;
	tok_it++;
	string attr_name = *tok_it;
	tok_it++;
	string type_id = *tok_it;
	tok_it++;
	string space_id = *tok_it;
	tok_it++;
	string acpl_id = *tok_it;

	long loc_id_int = boost::lexical_cast<long>(loc_id);
	string loc_id_str;
	if (loc_id_int == 0) {
		loc_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(loc_id_int);
		if (it == id_map.end()) {
			std::cerr << "H5Acreate1: \"" << loc_id << "\" loc id was not found in the map, ERROR?!" << std::endl;
			loc_id_str = loc_id;
		} else {
			loc_id_str = (it->second);
		}
	}

	string type_id_str;
	try {
		long type_id_int = boost::lexical_cast<long>(type_id);
		if (type_id_int == 0) {
			type_id_str = "0";
		} else {
			id_map_t::const_iterator it = id_map.find(type_id_int);
			if (it == id_map.end()) {
				std::cerr << "\"" << type_id << "\" type id was not found in the map, ERROR!" << std::endl;
				type_id_str = type_id;
			} else {
				type_id_str = (it->second);
			}
		}
	} catch (boost::bad_lexical_cast &) {
		type_id_str = type_id;
	}

	long space_id_int = boost::lexical_cast<long>(space_id);
	string space_id_str;
	if (space_id_int == 0) {
		space_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(space_id_int);
		if (it == id_map.end()) {
			std::cerr << "H5Acreate1: \"" << space_id << "\" space id was not found in the map, ERROR!" << std::endl;
		} else {
			space_id_str = (it->second);
		}
	}

	long acpl_id_int = boost::lexical_cast<long>(acpl_id);
	string acpl_id_str;
	if (acpl_id_int == 0) {
		acpl_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(acpl_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << acpl_id << "\" acpl id was not found in the map, ERROR!" << std::endl;
		} else {
			acpl_id_str = (it->second);
		}
	}

	long ret_id = boost::lexical_cast<long>(ret_value[0]);
	string ret_id_str;
	/*
	id_map_t::const_iterator it = id_map.find(ret_id);
	if(it == id_map.end()) {
		std::cerr << "\"" << ret_value[0] << "\" return id was not found in the map, adding it" << std::endl;
	*/
	ostringstream ret_id_stream;
	ret_id_stream << "hid_" << cur_id;
	cur_id++;
	ret_id_str = ret_id_stream.str();
	id_map.insert(std::pair<long, string>(ret_id, ret_id_str));
	/*
	}
	else {
		ret_id_str = (it->second);
	}
	*/

	ostringstream output_stream;
	output_stream << "hid_t " << ret_id_str << " = H5Acreate1(" << loc_id_str << ", \"" << attr_name << "\", " <<  type_id_str << ", " << space_id_str << ", " << acpl_id_str << ");" << std::endl;
	return output_stream.str();
}

string replay_H5Acreate2(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string loc_id = *tok_it;
	tok_it++;
	string attr_name = *tok_it;
	tok_it++;
	string type_id = *tok_it;
	tok_it++;
	string space_id = *tok_it;
	tok_it++;
	string acpl_id = *tok_it;
	tok_it++;
	string aapl_id = *tok_it;

	long loc_id_int = boost::lexical_cast<long>(loc_id);
	string loc_id_str;
	if (loc_id_int == 0) {
		loc_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(loc_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << loc_id << "\" loc id was not found in the map, ERROR!" << std::endl;
		} else {
			loc_id_str = (it->second);
		}
	}

	string type_id_str;
	try {
		long type_id_int = boost::lexical_cast<long>(type_id);
		if (type_id_int == 0) {
			type_id_str = "0";
		} else {
			id_map_t::const_iterator it = id_map.find(type_id_int);
			if (it == id_map.end()) {
				std::cerr << "\"" << type_id << "\" type id was not found in the map, ERROR!" << std::endl;
				type_id_str = type_id;
			} else {
				type_id_str = (it->second);
			}
		}
	} catch (boost::bad_lexical_cast &) {
		type_id_str = type_id;
	}

	long space_id_int = boost::lexical_cast<long>(space_id);
	string space_id_str;
	if (space_id_int == 0) {
		space_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(space_id_int);
		if (it == id_map.end()) {
			std::cerr << "H5Acreate2: \"" << space_id << "\" space id was not found in the map, ERROR!" << std::endl;
		} else {
			space_id_str = (it->second);
		}
	}

	long acpl_id_int = boost::lexical_cast<long>(acpl_id);
	string acpl_id_str;
	if (acpl_id_int == 0) {
		acpl_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(acpl_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << acpl_id << "\" acpl id was not found in the map, ERROR!" << std::endl;
		} else {
			acpl_id_str = (it->second);
		}
	}

	long aapl_id_int = boost::lexical_cast<long>(aapl_id);
	string aapl_id_str;
	if (aapl_id_int == 0) {
		aapl_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(aapl_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << aapl_id << "\" type id was not found in the map, ERROR!" << std::endl;
		} else {
			aapl_id_str = (it->second);
		}
	}

	long ret_id = boost::lexical_cast<long>(ret_value[0]);
	string ret_id_str;
	/*
	id_map_t::const_iterator it = id_map.find(ret_id);
	if(it == id_map.end()) {
		std::cerr << "\"" << ret_value[0] << "\" return id was not found in the map, adding it" << std::endl;
	*/
	ostringstream ret_id_stream;
	ret_id_stream << "hid_" << cur_id;
	cur_id++;
	ret_id_str = ret_id_stream.str();
	id_map.insert(std::pair<long, string>(ret_id, ret_id_str));
	/*
	}
	else {
		ret_id_str = (it->second);
	}
	*/

	ostringstream output_stream;
	output_stream << "hid_t " << ret_id_str << " = H5Acreate2(" << loc_id_str << ", \"" << attr_name << "\", " <<  type_id_str << ", " << space_id_str << ", " << acpl_id_str << ", " << aapl_id_str << ");" << std::endl;
	return output_stream.str();
}

string replay_H5Aget_name(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Aget_num_attrs(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Aget_space(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}
string replay_H5Aget_type(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Aopen(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Aopen_idx(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Aopen_name(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Aread(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Awrite(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string attr_id = *tok_it;
	tok_it++;
	string mem_type_id = *tok_it;

	long attr_id_int = boost::lexical_cast<long>(attr_id);
	string attr_id_str;
	if (attr_id_int == 0) {
		attr_id_str = "0";
	} else {
		id_map_t::const_iterator it = id_map.find(attr_id_int);
		if (it == id_map.end()) {
			std::cerr << "\"" << attr_id << "\" attr id was not found in the map, ERROR!" << std::endl;
		} else {
			attr_id_str = (it->second);
		}
	}

	string mem_type_id_str;
	try {
		long mem_type_id_int = boost::lexical_cast<long>(mem_type_id);
		if (mem_type_id_int == 0) {
			mem_type_id_str = "0";
		} else {
			id_map_t::const_iterator it = id_map.find(mem_type_id_int);
			if (it == id_map.end()) {
				std::cerr << "\"" << mem_type_id << "\" dtype id was not found in the map, USING THE NUMBER!" << std::endl;
				mem_type_id_str = mem_type_id;
			} else {
				mem_type_id_str = (it->second);
			}
		}
	} catch (boost::bad_lexical_cast &) {
		mem_type_id_str = mem_type_id;
	}

	ostringstream output_stream;

	string its_args[1], its_ret_value[1];
	its_args[0] = mem_type_id_str;
	output_stream << replay_H5Tget_size(ranks, same, rank_ids, diffArgs, loop_size, its_args, its_ret_value);
	string size_dtype_str = its_ret_value[0];

	string dummy_data_str;
	ostringstream dummy_data_id_stream;
	dummy_data_id_stream << "dummy_data_" << data_id;
	data_id++;
	dummy_data_str = dummy_data_id_stream.str();

	output_stream << "void *" << dummy_data_str << " = (void *) malloc(" << size_dtype_str << ");" << std::endl;

	allocated_data[attr_id_str] = dummy_data_str;

	output_stream << "H5Awrite(" << attr_id_str << ", " << mem_type_id_str << ", " << dummy_data_str << ");" << std::endl;

	output_stream << "free(" << dummy_data_str << ");" << std::endl;

	return output_stream.str();

}

string replay_H5Pclose(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	long plist_id = boost::lexical_cast<long>(args[0]);

	string plist_id_str;
	id_map_t::iterator it = id_map.find(plist_id);
	if (it == id_map.end()) {
		std::cerr << "\"" << plist_id << "\" plist id was not found in the map, ERROR!" << std::endl;
	} else {
		plist_id_str = (it->second);

		if (same) {
			id_map.erase(it);
		}
	}

	//fprintf(outputfh,"\n\tH5Pclose(%d);\n",plist_id);
	ostringstream output_stream;
	output_stream << "H5Pclose(" << plist_id_str << "); " << std::endl;
	return output_stream.str();
}

string replay_H5Pcreate(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	long ret_id = boost::lexical_cast<long>(ret_value[0]);

	string ret_id_str;
	/*
	id_map_t::const_iterator it = id_map.find(ret_id);
	if(it == id_map.end()) {
		std::cerr << "\"" << ret_value[0] << "\" return id was not found in the map, adding it" << std::endl;
	*/
	ostringstream ret_id_stream;
	ret_id_stream << "hid_" << cur_id;
	cur_id++;
	ret_id_str = ret_id_stream.str();
	id_map.insert(std::pair<long, string>(ret_id, ret_id_str));
	/*
	}
	else {
		ret_id_str = (it->second);
	}
	*/

	ostringstream output_stream;
	output_stream << "hid_t " << ret_id_str << " = H5Pcreate(" << args[0] << "); " << std::endl;
	return output_stream.str();
}

string replay_H5Pget_chunk(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Pget_mdc_config(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	// fprintf(outputfh,"\n\t%.5f H5Pget_mdc_config(%d, &config) = %d <%.5f> \n",tm1, plist_id, ret, duration);
	// fout << "H5Pget_mdc_config(" << args << ") = " << ret_value[0] << std::endl;
	return "\n";
}

string replay_H5Pset_mdc_config(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string plist_id = *tok_it;
	tok_it++;
	string config_ptr = *tok_it;

	string plist_id_name;
	id_map_t::const_iterator it = id_map.find(boost::lexical_cast<int>(plist_id));
	if (it == id_map.end()) {
		std::cerr << "\"" << ret_value[0] << "\" not found in the map! ERROR!" << std::endl;
	} else {
		plist_id_name = (it->second);
	}

	config_ptr.erase(config_ptr.begin(), config_ptr.begin() + 1);
	boost::char_separator<char> sep2(";");
	boost::tokenizer< boost::char_separator<char> > tok2(config_ptr, sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it2 = tok2.begin();
	H5AC_cache_config_t cache_config;

	return "";
#if 0
	long.version = boost::lexical_cast<long>(*tok_it2);
	fout << ">> " <<  *tok_it2 << std::endl;
	tok_it2++;
	long.rpt_fcn_enabled = boost::lexical_cast<long>(*tok_it2);
	tok_it2++;
	long.open_trace_file = boost::lexical_cast<long>(*tok_it2);
	tok_it2++;
	long.close_trace_file = boost::lexical_cast<long>(*tok_it2);
	tok_it2++;
	//cache_config.trace_file_name = *tok_it2;
	tok_it2++;
	long.evictions_enabled = boost::lexical_cast<long>(*tok_it2);
	tok_it2++;
	long.set_initial_size = boost::lexical_cast<long>(*tok_it2);
	tok_it2++;
	long.initial_size = boost::lexical_cast<long>(*tok_it2);
	tok_it2++;
	cache_config.min_clean_fraction = boost::lexical_cast<double>(*tok_it2);
	tok_it2++;
	long.max_size = boost::lexical_cast<long>(*tok_it2);
	tok_it2++;
	long.min_size = boost::lexical_cast<long>(*tok_it2);
	tok_it2++;
	cache_config.epoch_length = boost::lexical_cast<long>(*tok_it2);
	tok_it2++;
	long cache_config.incr_mode = boost::lexical_cast<long>(*tok_it2);
	tok_it2++;
	cache_config.lower_hr_threshold = boost::lexical_cast<double>(*tok_it2);
	tok_it2++;
	cache_config.increment = boost::lexical_cast<double>(*tok_it2);
	tok_it2++;
	long.apply_max_increment = boost::lexical_cast<long>(*tok_it2);
	tok_it2++;
	long.max_increment = boost::lexical_cast<long>(*tok_it2);
	tok_it2++;
	// cache_config.flash_incr_mode = boost::lexical_cast<int>(*tok_it2);
	tok_it2++;
	cache_config.flash_multiple = boost::lexical_cast<double>(*tok_it2);
	tok_it2++;
	cache_config.flash_threshold = boost::lexical_cast<double>(*tok_it2);
	tok_it2++;
	longcache_config.decr_mode = boost::lexical_cast<long>(*tok_it2);
	tok_it2++;
	cache_config.upper_hr_threshold = boost::lexical_cast<double>(*tok_it2);
	tok_it2++;
	cache_config.decrement = boost::lexical_cast<double>(*tok_it2);
	tok_it2++;
	long.apply_max_decrement = boost::lexical_cast<long>(*tok_it2);
	tok_it2++;
	cache_config.max_decrement = boost::lexical_cast<int>(*tok_it2);
	tok_it2++;
	long.epochs_before_eviction = boost::lexical_cast<long>(*tok_it2);
	tok_it2++;
	cache_config.apply_empty_reserve = boost::lexical_cast<int>(*tok_it2);
	tok_it2++;
	cache_config.empty_reserve = boost::lexical_cast<double>(*tok_it2);
	tok_it2++;
	cache_config.dirty_bytes_threshold = boost::lexical_cast<int>(*tok_it2);
	tok_it2++;
	cache_config.metadata_write_strategy = boost::lexical_cast<int>(*tok_it2);

	fout << "H5Pset_mdc_config(" << plist_id << ") to " << config_ptr << std::endl;
#endif
}

string replay_H5Pset_alignment(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Pset_chunk(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Pset_dxpl_mpio(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string dxpl_id = *tok_it;
	tok_it++;
	string xfer_mode = *tok_it;

	string dxpl_id_name;
	id_map_t::const_iterator it = id_map.find(boost::lexical_cast<int>(dxpl_id));
	if (it == id_map.end()) {
		std::cerr << "\"" << ret_value[0] << "\" not found in the map! ERROR!" << std::endl;
	} else {
		dxpl_id_name = (it->second);
	}

	ostringstream output_stream;
	output_stream << "H5Pset_dxpl_mpio(" << dxpl_id_name << ", " << xfer_mode << ");" << std::endl;
	return output_stream.str();
}

string replay_H5Pset_fapl_core(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Pset_fapl_mpio(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	boost::char_separator<char> sep(",");
	boost::tokenizer< boost::char_separator<char> > tok(args[0], sep);
	boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();
	string fapl_id = *tok_it;
	tok_it++;
	string comm = *tok_it;
	tok_it++;
	string info = *tok_it;

	string fapl_id_name;
	id_map_t::const_iterator it = id_map.find(boost::lexical_cast<int>(fapl_id));
	if (it == id_map.end()) {
		std::cerr << "\"" << ret_value[0] << "\" not found in the map! ERROR!" << std::endl;
	} else {
		fapl_id_name = (it->second);
	}

	ostringstream output_stream;

	string comm_str;
	ostringstream comm_stream;
	comm_stream << "comm_" << cur_id;
	comm_str = comm_stream.str();
	output_stream << "MPI_Comm " << comm_str << " = " << comm << ";" << std::endl;

	string info_str;
	ostringstream info_stream;
	info_stream << "info_" << cur_id;
	cur_id++;
	info_str = info_stream.str();
	output_stream << "MPI_Info " << info_str << ";" << std::endl;
	output_stream << "MPI_Info_create(&" << info_str << ");" << std::endl;

	output_stream << "H5Pset_fapl_mpio(" << fapl_id_name << ", " << comm_str << ", " << info_str << ");" << std::endl;
	return output_stream.str();
}

string replay_H5Pset_fapl_mpiposix(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Pset_istore_k(string args, string ret_value) {
	return "\n";
}

string replay_H5Pset_meta_block_size(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Lexists(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Lget_val(string args, string ret_value) {
	return "\n";
}

string replay_H5Literate(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Oclose(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Oget_info(string args, string ret_value) {
	return "\n";
}

string replay_H5Oget_info_by_name(int ranks, bool same, int rank_ids[], bool diffArgs, int loop_size, string args[], string ret_value[]) {
	return "\n";
}

string replay_H5Oopen(string args, string ret_value) {
	return "\n";
}


string preamble() {
	ostringstream output_stream;

	output_stream << "#include <stdio.h>" << std::endl;
	output_stream << "#include <string.h>" << std::endl;
	output_stream << "#include <stdlib.h>" << std::endl;
	output_stream << "#include \"mpi.h\"" << std::endl;
	output_stream << "#include \"hdf5.h\"" << std::endl;

	output_stream << "\nint main(int argc, char* argv[])" << std::endl;
	output_stream << "{" << std::endl;

	output_stream << "int mpi_rank, mpi_size;" << std::endl;
	output_stream << "MPI_Init(&argc, &argv);" << std::endl;
	output_stream << "MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);" << std::endl;
	output_stream << "MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);\n" << std::endl;

	output_stream << "int loop_index;\n" << std::endl;

	return output_stream.str();
}

string postamble() {
	ostringstream output_stream;

	output_stream << "\nMPI_Finalize();" << std::endl;
	output_stream << "return 0;" << std::endl;
	output_stream << "}" << std::endl;

	return output_stream.str();
}
