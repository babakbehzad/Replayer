#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <assert.h>
#include <fstream>

#include <sstream>
#include <iterator>

const int oo = 1<<25;
const int MAXN = 40000;

using namespace std;

char *s;
int pos;

typedef std::map<char, int> next_t;

struct result_t {
  int start, d, count;
  result_t(int _start, int _d, int _count):start(_start), d(_d), count(_count) {};
};

//vector< pair<int,int> > mark;
vector<result_t*> result;
vector<bool> mark;

struct node_t {
	int start, end, slink, prelen, id;
	int dfs_left, dfs_right;
  next_t next;

	int edge_length() {
		return min(end, pos + 1) - start;
	}

	bool is_leaf() {
	  return end == oo;
  }

  int leaf_label() {
    return (pos + 1) - (prelen + edge_length());
  }
};

class SuffixTree {
public:
  SuffixTree() {
    needSL = 0, last_added = 0, pos = -1,
           remainder = 0, active_node = 0, active_e = 0, active_len = 0;
    root = active_node = new_node(-1, -1);
    _tree[root].prelen = 0;
  }

  void extend(int ch); // vocab convert to int

  node_t& node(int id) {
    return _tree[id];
  }

  int new_node(int start, int end = oo) {
    node_t nd;
    nd.start = start;
    nd.end = end;
    nd.slink = 0;
    nd.next.clear();
    nd.id = last_added;
    add(nd);
    return nd.id;
  }

  int root;
  int text[MAXN];

private:
  char active_edge();
  void add_SL(int nodeid);
  bool walk_down(int nodeid);
  void add(node_t node) { _tree[last_added++] = node; };

  node_t _tree[2*MAXN];
  int last_added, needSL, remainder, active_node, active_e, active_len;
};

SuffixTree *tree;

int DFS[MAXN];

char SuffixTree::active_edge() {
	return text[active_e];
}

void SuffixTree::add_SL(int node) {
	if (needSL > 0) _tree[needSL].slink = node;
	needSL = node;
}

bool SuffixTree::walk_down(int node) {
	if (active_len >= _tree[node].edge_length()) {
		active_e += _tree[node].edge_length();
		active_len -= _tree[node].edge_length();
		active_node = node;
		return true;
	}
	return false;
}

// tree construction, hardest part
void SuffixTree::extend(int c) {
	text[++pos] = c;
	needSL = 0;
	remainder++;
	while(remainder > 0) {
		if (active_len == 0) active_e = pos;
		if (_tree[active_node].next[active_edge()] == 0) {
			int leaf = new_node(pos);
			_tree[active_node].next[active_edge()] = leaf;
			_tree[leaf].prelen = _tree[active_node].edge_length() + _tree[active_node].prelen;
			add_SL(active_node); //rule 2
		} else {
			int nxt = _tree[active_node].next[active_edge()];
			if (walk_down(nxt)) continue; //observation 2
			if (text[_tree[nxt].start + active_len] == c) { //observation 1
				active_len++;
				add_SL(active_node); //observation 3
				break;
			}
			int split = new_node(_tree[nxt].start, _tree[nxt].start + active_len);

      _tree[split].prelen = _tree[active_node].prelen + _tree[active_node].edge_length();
			_tree[active_node].next[active_edge()] = split;

			int leaf = new_node(pos);

			_tree[leaf].prelen = _tree[split].prelen + _tree[split].edge_length();
			_tree[split].next[c] = leaf;

			_tree[nxt].start += active_len;

      _tree[nxt].prelen = _tree[split].prelen + _tree[split].edge_length();
			_tree[split].next[text[_tree[nxt].start]] = nxt;

			add_SL(split); //rule 2
		}
		remainder--;
		if (active_node == root && active_len > 0) { //rule 1
			active_len--;
			active_e = pos - remainder + 1;
		} else
			active_node = _tree[active_node].slink > 0 ? _tree[active_node].slink : root; //rule 3
	}
}

int count = 0;
void dfs_number(int nodeid) {
  tree->node(nodeid).dfs_left = count;
  if (tree->node(nodeid).is_leaf()) {
    DFS[tree->node(nodeid).leaf_label()] = count;
    tree->node(nodeid).dfs_right = count;
    count++;
    return;
  }

  for (next_t::iterator it = tree->node(nodeid).next.begin(), e = tree->node(nodeid).next.end();\
      it!=e; ++it) {
    int anode = it->second;
    dfs_number(anode);
  }
  tree->node(nodeid).dfs_right = count-1;
  assert(tree->node(nodeid).dfs_right >= tree->node(nodeid).dfs_left);
}

void tandem(int nodeid, std::vector<int> &LL, bool run=true) {
  // if leaf add it to the list
  if (tree->node(nodeid).is_leaf()) {
    LL.push_back(tree->node(nodeid).leaf_label());
    count ++;
    return;
  }

  int start = LL.size();

  // collect LL
  // Step 1: find "bignode"
  int max = -1;
  int bignode = -1;
  assert(tree->node(nodeid).next.size() > 0);

  for (next_t::iterator it = tree->node(nodeid).next.begin(), e = tree->node(nodeid).next.end();\
      it!=e; ++it) {
    int anode = it->second;
    if (tree->node(anode).dfs_right - tree->node(anode).dfs_left > max) {
      max = tree->node(anode).dfs_right - tree->node(anode).dfs_left;
      bignode = anode;
    }
  }
  assert(bignode != -1);

  // Step 2: compute tandem for child
  for (next_t::iterator it = tree->node(nodeid).next.begin(), e = tree->node(nodeid).next.end();\
      it!=e; ++it) {
    int anode = it->second;
    if (anode != bignode)
      tandem(anode, LL, false);
  }

  int end = LL.size();
  tandem(bignode, LL, false);
  // Step 3: LL' = LL - LL(bignode)
  //
  if (!run) return;

  // process the LL'
  for (int i=start; i<end; i++) {
    int i1 = LL[i];
    int d = tree->node(nodeid).prelen + tree->node(nodeid).edge_length();
    if (d == 0) continue;
    assert(d > 0);
    int i2 = i1 + d;

    // look to the right
    if (!mark[i1] && DFS[i2] >= tree->node(nodeid).dfs_left && DFS[i2] <= tree->node(nodeid).dfs_right\
       && tree->text[i1] != tree->text[i1+2*d]) { // second condition, only get the branching

      i2 = i1;
      while (i2-d>=0) {
        i2 -= d;
        if (!(DFS[i2] >= tree->node(nodeid).dfs_left && DFS[i2] <= tree->node(nodeid).dfs_right)) {
          i2 += d;
          break;
        }
        mark[i2] = 1;
      }

      //std::cout << "case 1: " << i2 << " " << i1 + d << " " << d << " " << (i1+d-i2)/d << endl;
      if(i2!=i1) result.push_back(new result_t(i2, d, (i1+d-i2)/d));
      mark[i2] = 1;
      //mark[i1+d] = make_pair(i2, d);
    }

    // look to the left
    i2 = i1 - d;
    if (i2 < 0) continue;

    if (!mark[i2] && DFS[i2] >= tree->node(nodeid).dfs_left && DFS[i2] <= tree->node(nodeid).dfs_right\
       && tree->text[i2] != tree->text[i2+2*d]) {

      while (i2-d>=0) {
        i2 -= d;
        if (!(DFS[i2] >= tree->node(nodeid).dfs_left && DFS[i2] <= tree->node(nodeid).dfs_right)) {
          i2 +=d;
          break;
        }
        mark[i2] = 1;
      }

      //std::cout << "case 2:" << i2 << " " << i1 + d << " " << d << " " << (i1+d-i2)/d << std::endl;
      result.push_back(new result_t(i2, d, (i1+d-i2)/d));
      //mark[i1+d] = make_pair(i2, d);
      mark[i2] = 1;
    }
  }

  LL.clear();
  for (next_t::iterator it = tree->node(nodeid).next.begin(), e = tree->node(nodeid).next.end();\
      it!=e; ++it) {
    int anode = it->second;
    if (!mark[tree->node(anode).start])
      tandem(anode, LL, true);
  }
}

void find_tendam_array() {
  count = 0;
  dfs_number(tree->root);
  std::vector<int> LL;
  tandem(tree->root, LL);
}

vector<string> everything;
vector<int> func2everything;
vector<pair<int,int> > toBeRemoved;

string getfield(const string &s,const char *field, const char *delim=",") {
  int p1 = s.find(field) + strlen(field) + 1;
  int p2 = s.find(delim, p1);
  return s.substr(p1, p2-p1);
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  split(s, delim, elems);
  return elems;
}

std::vector<std::string> getargs(const std::string &s) {
  string args0 = getfield(s, "args=", "]");
  return split(args0, ',');
}


void print_r(int start, int end, int loop) {
  int jump = (end-start) / loop;
  vector<string> new_merg;

  stringstream ss;
  ss << "{ loop:" << loop << " }";

  new_merg.push_back(ss.str());

  for (int pos=0; pos<jump; pos++) {
    string startp = everything[start+pos];
    bool diff = false;
    for (int i=start+pos; i<end; i+=jump) {
      if (everything[i] != startp) {
        diff = true;
        break;
      }
    }
    if (diff) {
      // when things are different, pack all into args
      int p1 = startp.find("args=") + 5;
      int p2 = startp.find("]", p1);

      stringstream ss;
      ss << startp.substr(0, p1) << "[";

      vector<string> args0 = getargs(everything[start+pos]);
      vector<stringstream> s_args(args0.size());
      vector<bool> diff_args(args0.size(), false);

      for (int i=start+pos; i<end; i+=jump) {
        //ss << getfield(everything[i], "args", "]") << "];";
        vector<string> argsi = getargs(everything[i]);
        for (size_t j=0; j<argsi.size(); j++){
          s_args[j] << argsi[j] << ";";
          if (argsi[j] != args0[j]) diff_args[j] = true;
        }
      }

      for (size_t j=0; j<args0.size(); j++){
        if (!diff_args[j]) {
          ss << args0[j] << ",";
        } else {
          ss << "<" << s_args[j].str() << ">,";
        }
      }

      ss << "]" << startp.substr(p2+1, startp.length() - p2);

      new_merg.push_back(ss.str());
    } else {
      new_merg.push_back(startp);
    }
  }

  new_merg.push_back("{ endloop }");

  for (int i=start; i<end;i++) {
    everything[i] = "";
  }

  for (size_t i=0; i<new_merg.size(); i++) {
    everything[start + i] = new_merg[i];
  }

}

int main(int argc, char **args) {
  if (argc < 3) {
    cerr << "Usage: " << args[0] << " <merged file> <nprocs>" << endl;
    return -1;
  }

  tree = new SuffixTree();
  mark.resize(MAXN, false);
#if 1
  string s;
  std::map<string, int> dict;
  vector<string> vocab;
  mark.resize(MAXN, 0);

  ifstream fin(args[1]);
  int nprocs = atoi(args[2]);
  string funcname = "";
  getline(fin, s);
  everything.push_back(s);
  string key = s;
  func2everything.push_back(1);
  getline(fin, s);
  while (fin) {
    everything.push_back(s);
    if (s.find("same=") != string::npos) {
      func2everything.push_back(everything.size());
      int id;
      key += funcname;
      if (dict.find(key) == dict.end()) {
        id = vocab.size();
        dict[key] = id;
        vocab.push_back(key);
        // cout << "Adding " << key << " " << everything.size() << endl;
      } else {
        id = dict[key];
      }
      tree->extend(id);
      key = s;
      funcname = "";
    } else {
      funcname = "_" + getfield(s, "func");
      funcname += "_" + getfield(s, "file");
    }
    getline(fin, s);
  }
#else
  s = args[1];
  for (int i=0; i<strlen(s);i++)
    tree->extend(s[i]);
#endif
  tree->extend('$');
  result.clear();
  find_tendam_array();

  for (size_t i=0; i<result.size();i++) {
    result_t *r = result[i];
    if (r->count > 20) {
      int end = r->start + r->d * r->count;
      cout << r->start << " " << r->d << " " << r->count \
        << " " << func2everything[r->start] << " " << func2everything[end] << endl;
      print_r(func2everything[r->start]-1, func2everything[end]-1, r->count); // -1 since count from 0
    }
  }

  ofstream fout(string(args[1]) + ".min");

  stringstream buffer;
  string current = "";
  bool first = true;

  buffer << "{ same=" << nprocs;
  string signature = buffer.str();
  buffer.str( std::string() );
  buffer.clear();

  for (size_t i=0; i<everything.size();i++) {
    if (everything[i] == "") continue;

    //loop = everything[i].find("{ loop") != string::npos || everything[i].find("{ endloop }") != string::npos;

    if (everything[i].find("{ same=") != string::npos) {
      if (!first && (current != everything[i])){ // || loop)) {
        if (current.find(signature) == string::npos) {
          fout << "{ scope }" << endl;
          fout << buffer.str();
          fout << "{ endscope }" << endl;
        } else
          fout << buffer.str();
        buffer.str( std::string() );
        buffer.clear();
      }
      current = everything[i];
      first = false;
    }

    buffer << everything[i] << endl;
  }

  if (current.find(signature) == string::npos) {
    fout << "{ scope }" << endl;
    fout << buffer.str();
    fout << "{ endscope }" << endl;
  } else
    fout << buffer.str();

  fout.close();

	return 0;
}
