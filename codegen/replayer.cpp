#include "replayer.hpp"

using namespace std;

ifstream fin;
ofstream fout;

func_map_t func_map;
id_map_t id_map;

int num_procs;

int cur_id;
int dim_id;
int data_id;

allocated_data_map_t allocated_data;
allocated_rank_map_t allocated_rank;

void populate_func_map() {
	func_map["H5Fcreate"] = &replay_H5Fcreate;
	func_map["H5Fopen"] = &replay_H5Fopen;
	func_map["H5Fclose"] = &replay_H5Fclose;
	func_map["H5Gclose"] = &replay_H5Gclose;
	func_map["H5Gcreate1"] = &replay_H5Gcreate1;
	func_map["H5Gcreate2"] = &replay_H5Gcreate2;
	func_map["H5Gget_objinfo"] = &replay_H5Gget_objinfo;
	func_map["H5Giterate"] = &replay_H5Giterate;
	func_map["H5Gopen1"] = &replay_H5Gopen1;
	func_map["H5Gopen2"] = &replay_H5Gopen2;
	func_map["H5Dclose"] = &replay_H5Dclose;
	func_map["H5Dcreate1"] = &replay_H5Dcreate1;
	func_map["H5Dcreate2"] = &replay_H5Dcreate2;
	func_map["H5Dget_create_plist"] = &replay_H5Dget_create_plist;
	func_map["H5Dget_space"] = &replay_H5Dget_space;
	func_map["H5Dget_type"] = &replay_H5Dget_type;
	func_map["H5Dopen1"] = &replay_H5Dopen1;
	func_map["H5Dopen2"] = &replay_H5Dopen2;
	func_map["H5Dread"] = &replay_H5Dread;
	func_map["H5Dwrite"] = &replay_H5Dwrite;
	func_map["H5Sclose"] = &replay_H5Sclose;
	func_map["H5Screate"] = &replay_H5Screate;
	func_map["H5Screate_simple"] = &replay_H5Screate_simple;
	func_map["H5Sget_select_npoints"] = &replay_H5Sget_select_npoints;
	func_map["H5Sget_simple_extent_dims"] = &replay_H5Sget_simple_extent_dims;
	func_map["H5Sget_simple_extent_npoints"] = &replay_H5Sget_simple_extent_npoints;
	func_map["H5Sselect_elements"] = &replay_H5Sselect_elements;
	func_map["H5Sselect_hyperslab"] = &replay_H5Sselect_hyperslab;
	func_map["H5Sselect_none"] = &replay_H5Sselect_none;
	func_map["H5Tclose"] = &replay_H5Tclose;
	func_map["H5Tcreate"] = &replay_H5Tcreate;
	func_map["H5Topen1"] = &replay_H5Topen1;
	func_map["H5Topen2"] = &replay_H5Topen2;
	func_map["H5Tcommit1"] = &replay_H5Tcommit1;
	func_map["H5Tcommit2"] = &replay_H5Tcommit2;
	func_map["H5Tinsert"] = &replay_H5Tinsert;
 	func_map["H5Tcopy"] = &replay_H5Tcopy;
	func_map["H5Tget_class"] = &replay_H5Tget_class;
	func_map["H5Tget_size"] = &replay_H5Tget_size;
	func_map["H5Tset_size"] = &replay_H5Tset_size;
	func_map["H5Aclose"] = &replay_H5Aclose;
	func_map["H5Acreate1"] = &replay_H5Acreate1;
	func_map["H5Acreate2"] = &replay_H5Acreate2;
	func_map["H5Aget_name"] = &replay_H5Aget_name;
	func_map["H5Aget_num_attrs"] = &replay_H5Aget_num_attrs;
	func_map["H5Aget_space"] = &replay_H5Aget_space;
	func_map["H5Aget_type"] = &replay_H5Aget_type;
	func_map["H5Aopen"] = &replay_H5Aopen;
	func_map["H5Aopen_idx"] = &replay_H5Aopen_idx;
	func_map["H5Aopen_name"] = &replay_H5Aopen_name;
	func_map["H5Aread"] = &replay_H5Aread;
	func_map["H5Awrite"] = &replay_H5Awrite;
	func_map["H5Pclose"] = &replay_H5Pclose;
	func_map["H5Pcreate"] = &replay_H5Pcreate;
	func_map["H5Pget_chunk"] = &replay_H5Pget_chunk;
	func_map["H5Pget_mdc_config"] = &replay_H5Pget_mdc_config;
	func_map["H5Pset_mdc_config"] = &replay_H5Pset_mdc_config;
	func_map["H5Pset_alignment"] = &replay_H5Pset_alignment;
	func_map["H5Pset_chunk"] = &replay_H5Pset_chunk;
	func_map["H5Pset_dxpl_mpio"] = &replay_H5Pset_dxpl_mpio;
	func_map["H5Pset_fapl_core"] = &replay_H5Pset_fapl_core;
	func_map["H5Pset_fapl_mpio"] = &replay_H5Pset_fapl_mpio;
}

void work() {
	cur_id = 0;
	dim_id = 0;
	data_id = 0;

	bool in_scope = false;

	int loop_size = 0;

	vector<string> declarations;
	vector<string> statements;
	vector<string> processors;

	for (string line; getline(fin, line);) {
		cout << "LINE: " << line << endl;

		if((line.find("endloop")) != string::npos) {
			fout << "} " << std::endl;

			loop_size = 0;

			continue;
		}
		else if((line.find("loop")) != string::npos) {
			std::size_t collon = line.find(":");
			string loop_sz = line.substr(collon + 1, line.find("}") - collon - 2);
			loop_size = boost::lexical_cast<int>(loop_sz);

			fout << "for(loop_index = 0; loop_index < " << loop_size << "; loop_index++) { " << std::endl;

			continue;
		}
		else if((line.find("endscope")) != string::npos) {
			in_scope = false;
			if(processors.size()) {
				int i = 0;
				for(i = 0; i < declarations.size(); i++)
					fout << declarations[i];

					fout << "if(";
					for(i = 0; i < processors.size() - 1; i++) {
						fout << "mpi_rank == " << processors[i] << " || ";
					}
					fout << "mpi_rank == " << processors[i];
					fout << ") {" << std::endl;

				for(i = 0; i < statements.size(); i++)
					fout << statements[i];

				fout << "}" << std::endl;
			}

			continue;
		}
		else if((line.find("scope")) != string::npos) {
			in_scope = true;

			declarations.clear();
			statements.clear();
			processors.clear();

			continue;
		}

		std::size_t first_equal = line.find("=");
		string same = line.substr(first_equal + 1, line.find(":") - first_equal - 1);
		int cur_same = boost::lexical_cast<int>(same);

		std::size_t second_equal = line.find("=", first_equal+1);
		string diffarg = line.substr(second_equal + 1, 1);
		int cur_diffarg = boost::lexical_cast<int>(diffarg);

		std::size_t third_equal = line.find("=", second_equal+1);
		string diffret = line.substr(third_equal + 1, 1);
		int cur_diffret = boost::lexical_cast<int>(diffret);

		string func_line;
		if(cur_same == num_procs) {
			std::getline(fin, func_line);

			string func_name;
			int start = func_line.find("func=") + 5;
			int end = func_line.find(",", start);
			func_name = func_line.substr(start, end - start);

			string *func_args;
			string *func_ret_value;
			if(cur_diffarg == 0) {
				start = func_line.find("args=") + 6;
				end = func_line.find("]", start);
				func_args = new string[1];
				func_args[0] = func_line.substr(start, end - start);

				string *func_ret_value;
				start = func_line.find("R=[ ") + 4;
				end = func_line.find("]", start) - 1;
				func_ret_value = new string[1];
				func_ret_value[0] = func_line.substr(start, end - start);

				// std::cout << func_name << " : " << func_args << std::endl;
				func_map_t::const_iterator it = func_map.find(func_name);
				if(it == func_map.end()) {
					std::cerr << "\"" << func_name << "\" function not found!" << std::endl;
				}
				else {
					fout << (*it->second)(cur_same, true, NULL, cur_diffarg, loop_size, func_args, func_ret_value);
				}
			}
			else {
				func_args = new string[num_procs];
				func_ret_value = new string[num_procs];

				start = func_line.find("args=") + 6;
				end = func_line.find("]", start);
				func_args[0] = func_line.substr(start, end - start);

				start = func_line.find("R=[ ") + 4;
				end = func_line.find("]", start) - 1;
				func_ret_value[0] = func_line.substr(start, end - start);

				for(int i = 1; i < cur_same; i++) {
					std::getline(fin, func_line);

					start = func_line.find("args=") + 6;
					end = func_line.find("]", start);
					func_args[i] = func_line.substr(start, end - start);

					start = func_line.find("R=[ ") + 4;
					end = func_line.find("]", start) - 1;
					func_ret_value[i] = func_line.substr(start, end - start);
				}

				func_map_t::const_iterator it = func_map.find(func_name);
				if(it == func_map.end()) {
					std::cerr << "\"" << func_name << "\" function not found!" << std::endl;
				}
				else {
					fout << (*it->second)(cur_same, true, NULL, cur_diffarg, loop_size, func_args, func_ret_value);
				}
			}
		}
		else {
			int cur_same = boost::lexical_cast<int>(same);

			std::size_t first_space = line.find(" ");
			std::size_t second_space = line.find(" ", first_space+1);
			std::size_t third_space = line.find(" ", second_space+1);
			string the_procs = line.substr(second_space + 1, third_space - second_space - 1);
			//std::cout << "the_procs = " << the_procs << endl;

			boost::char_separator<char> sep(", ");
			boost::tokenizer< boost::char_separator<char> > tok(the_procs, sep);
			boost::tokenizer< boost::char_separator<char> >::iterator tok_it = tok.begin();

			if(processors.empty()) {
				for(tok_it = tok.begin(); tok_it != tok.end(); ++tok_it) {
					//cout << "pushing_back(" << *tok_it << ")" << endl;
					processors.push_back(*tok_it);
				}
			}

			std::getline(fin, func_line);
			// cout << func_line << endl;

			string func_name;
			int start = func_line.find("func=") + 5;
			int end = func_line.find(",", start);
			func_name = func_line.substr(start, end - start);

			string *func_args;
			string *func_ret_value;
			if(cur_diffarg == 0) {
				start = func_line.find("args=") + 6;
				end = func_line.find("]", start);
				func_args = new string[1];
				func_args[0] = func_line.substr(start, end - start);

				start = func_line.find("R=[ ") + 4;
				end = func_line.find("]", start) - 1;
				func_ret_value = new string[1];
				func_ret_value[0] = func_line.substr(start, end - start);
			}
			else {
				func_args = new string[num_procs];
				func_ret_value = new string[num_procs];

				start = func_line.find("args=") + 6;
				end = func_line.find("]", start);
				func_args[0] = func_line.substr(start, end - start);

				start = func_line.find("R=[ ") + 4;
				end = func_line.find("]", start) - 1;
				func_ret_value[0] = func_line.substr(start, end - start);

				for(int i = 1; i < cur_same; i++) {
					std::getline(fin, func_line);

					start = func_line.find("args=") + 6;
					end = func_line.find("]", start);
					func_args[i] = func_line.substr(start, end - start);

					start = func_line.find("R=[ ") + 4;
					end = func_line.find("]", start) - 1;
					func_ret_value[i] = func_line.substr(start, end - start);
				}

			}

			//std::cout << func_name << " : " << func_args << std::endl;
			func_map_t::const_iterator it = func_map.find(func_name);
			if(it == func_map.end()) {
				std::cerr << "\"" << func_name << "\" function not found!" << std::endl;
			}
			else {
				int *rank_ids = new int[processors.size()];
				for(int i = 0; i < processors.size(); i++) {
					rank_ids[i] = boost::lexical_cast<int>(processors[i]);
				}

				string stmts = (*it->second)(cur_same, false, rank_ids, true, loop_size, func_args, func_ret_value);

				boost::char_separator<char> sep_stmts(";\n");
				boost::tokenizer< boost::char_separator<char> > tok_stmts(stmts, sep_stmts);
				boost::tokenizer< boost::char_separator<char> >::iterator tok_stmts_it = tok_stmts.begin();
				for(tok_stmts_it = tok_stmts.begin(); tok_stmts_it != tok_stmts.end(); ++tok_stmts_it) {
					string stmt = *tok_stmts_it;
					int found = stmt.find("=");
					if(found != string::npos && stmt.find("[") == string::npos) {
						// It is a declaration statement: Have to break it to two parts
						string decl = stmt.substr(0, found) + ";";
						declarations.push_back(decl);

						int stmt_first_space = stmt.find(" ");
						string defn = stmt.substr(stmt_first_space) + ";";
						int star = defn.find("*");
						if(star == 1) {
							defn.erase(1, 1);
						}
						statements.push_back(defn);
					}
					else {
						// It is not a declaration statement
						statements.push_back(stmt + ";");
					}
				}
			}

		}
	}
}

int main(int argc, char *argv[]) {
	string inputfile_name;
	string outputfile_name;

	if (argc != 4) {
		cout << "Usage: combine inputfile outputfile $n\n" << endl;
		return 0;
	}

	inputfile_name.assign(argv[1]);
	outputfile_name.assign(argv[2]);

	fin.open(argv[1]);
	fout.open(argv[2]);
	num_procs = boost::lexical_cast<int>(argv[3]);

	if (fin.fail()) {
		cout << "Bad file name: [" << inputfile_name << "]!\n" << endl;
		return 0;
	}

	fout << preamble();

	populate_func_map();

	work();

	fin.close();
	fin.clear();

	fout << postamble();


	fout.close();
	fout.clear();

	return 0;
}

