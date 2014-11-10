/* Realize the merging algorithm */

/* Authors:
   - Weizhe and his student (for the original algorithm + code)
   - Farah Hariri (for the new algorithm and new code)
   - Hoang-Vu Dang (for the new algorithm and refactoring */

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <unordered_map>
#include <queue>
#include <assert.h>

#define MAX_DEPTH 200
#define INF 10e9
#define MAX_POSSIBLE_INDEX 10e7

//#define PRINT_DEBUG

using namespace std;

typedef vector <string> VS;
typedef vector<queue<int>> queue_vec;

ifstream input_is;
ofstream res_os;

struct Tuple {
  int sum;
  queue_vec indices;
};

/* this class is to store a log entry, it decomposed func and params for re-using later */
class LogEntry {
  private:
    string line;
    string funcName;
    VS params;
    string key;
    string strPara;

    VS _getPara(const string &a);
    string _getFunc(const string &a);
    string _getKey(const string &a);

  public:
    LogEntry(string _line):line(_line) {
      params = _getPara(line);
      funcName = _getFunc(line);
      key = _getKey(line);
    }

    // this is the key used for global hash as well as comparison
    // temporaryly it's just the function name
    string &getKey() { return key; };

    string &getLine() { return line; };

    string &getFunc() { return funcName; };

    string &getStrPara() { return strPara; };

    VS &getPara() { return params; };

    double getTime(const string &key);
    string getVal(const string &key);

    friend bool operator == (LogEntry &p1, LogEntry &p2) {
      return p1.getKey() == p2.getKey();
    }

    friend bool operator != (LogEntry &p1, LogEntry &p2) {
      return !(p1 == p2);
    }

    friend std::ostream& operator<< (std::ostream& stream, LogEntry &p) {
      stream << "func=" << p.funcName << ", ";

      VS para = p.params;
      stream << "argc=" << para.size() << ", ";
      stream << "args=[";
      for(unsigned j = 0; j < para.size() - 1; j++)
        stream << para[j] << ",";
      stream << para[para.size() - 1] << "], ";
#if GEN_DETAIL
      stream << p.getVal("S=[") << ", ";
      stream << p.getVal("E=[") << ", ";
      stream << p.getVal("D=[") << ", ";
#endif
      stream << p.getVal("R=[") << ", ";
      return stream;
    }
};

typedef vector<LogEntry *> VE;
vector<VE> logs;

/* this data structure is to keep track of log entry in all traces */
class GlobalHash {

  private:
    unordered_map< string, Tuple > my_hashmap;
    int numProcs;

  public:
    void insert(string &key, unsigned, unsigned);
    void remove(string &key, unsigned, unsigned);
    int getOptVal(LogEntry *);

    GlobalHash(int n): numProcs(n) {};
};

/*Get the parameter starting with key in record a*/
VS LogEntry::_getPara(const string &a)
{
  int start, end, i;
  start= a.find('(');
  start++;
  i = start;
  end=a.find(')');

  this->strPara = a.substr(start, end-start);

  VS result;

  while(i < end) {
    if(a[i] == ',') {
      result.push_back(a.substr(start, i-start));
      start = i + 1;
    }
    i++;
  }
  result.push_back(a.substr(start, i-start));

  return result;
}

// TODO: make this func more generic ?
string LogEntry::_getKey(const string &line) {

  stringstream s;

  s << funcName << "(";
  for (VS::iterator i = params.begin(), ie = params.end(); i != ie; i++) {
    string p = *i;

    // should not compare any pointer / array type
    if (p.find("0x") == string::npos && p.find("{") == string::npos)
      s << p;
    else
      s << "--"; // this is a field that is not used in the comparison
    if (i + 1 != ie)
      s << ",";
  }
  s << ")";

  return s.str();
}

/*Get the time starting with key in record a and return in string type*/
string LogEntry::getVal(const string &key)
{
  int start, end;

  start= line.find(key);
  end=start;
  while (line[end]!=']')
    end++;
  return line.substr(start, end-start+1);
}

/*Get the time starting with key in record a and return in double type*/
double LogEntry::getTime(const string &key)
{
  int start, end;
  double re;

  start= line.find(key);
  end=start;
  while (line[end]!=']')
    end++;
  while (line[start]!='[')
    start++;

  sscanf(line.substr(start+1, end-start).c_str(), "%lf", &re);

  return re;
}

/*Get the name of the function in record s*/
string LogEntry::_getFunc(const string &s)
{
  int start,end;

  start=0;
  end=s.find("=")-1;
  return s.substr(start, end - start + 1);
}

/*Show the records that can be merged in a unified format, including the file number and some parameters of these records*/
void showFunc(vector<bool> &merge, vector <unsigned> &p)
{
  string func;
  int n = logs.size();

  int count = 0;
  for (int i=0; i< n; i++)
    count += merge[i];

  res_os << "{ same=" << count << ": ";
  if(count != n) {
	  for (int i = 0; i < n; i++) {
		if (merge[i])
		  res_os << i << ",";
	  }
  }
  res_os << " ";

  string para0, ret0;
  for (int i=0; i < n; i++) {
    if (merge[i]) {
      assert( p[i] < logs[i].size() && "for safety purpose, but this should not happen");
      para0 = logs[i][p[i]]->getStrPara();
      ret0 = logs[i][p[i]]->getVal("R=[");
    }
  }

  // check if params are different
  bool diffp = false;
  for (int i=0; i < n; i++) {
    if (merge[i]) {
      assert( p[i] < logs[i].size() && "for safety purpose, but this should not happen");
      if (logs[i][p[i]]->getStrPara() != para0) {
        diffp = true;
        break;
      }
    }
  }

  // check if return are different
  bool diffr = false;
  for (int i=0; i < n; i++) {
    if (merge[i]) {
      assert( p[i] < logs[i].size() && "for safety purpose, but this should not happen");
      if (logs[i][p[i]]->getVal("R=[") != ret0) {
        diffr = true;
        break;
      }
    }
  }

  res_os << "diffarg=" << (diffp?1:0) << " diffret=" << (diffr?1:0) << " } " << endl;

  for (int i=0; i < n; i++) {
    if (merge[i]) {
      res_os << "{ file=" << i << ", ";
      res_os << *logs[i][p[i]];
#if GEN_DETAIL
      res_os << "Blank=[ ";
      // res_os.precision(10);
      if (p[i] == 0)
        res_os << 0;
      else
        //Calculate the computation time between two adjacent communication function
        res_os << fixed << logs[i][p[i]]->getTime("S=[") - logs[i][p[i]-1]->getTime("E=[");
      res_os << " ], }" << endl;
      if (!diffp && !diffr); // to avoid warning
#else
      res_os << "}" << endl;
      if (!diffp && !diffr) break; // if same params + same return, avoid output
#endif
    }
  }
}

int GlobalHash::getOptVal(LogEntry *entry) {
  unordered_map<string, Tuple>::iterator hm_it;
  hm_it = my_hashmap.find(entry->getKey());
  return hm_it->second.sum;
}


void GlobalHash::insert(string &key, unsigned pid, unsigned index)
{
#ifdef PRINT_DEBUG
  cout << "Inserting ... " << key;
#endif

  //check if entry corresponding the function signature already exists in the hashtable
  unordered_map<string, Tuple>::iterator hm_it;
  hm_it=my_hashmap.find(key);

  //if entry does not exist:
  if(hm_it == my_hashmap.end())
  {
    //in this case we need to create a new entry to add it
    //create a new vector of queues
    queue_vec v(numProcs);

    //push the current element to be added into its queue
    v[pid].push(index);

    //create a tuple to be added to the hashtable
    Tuple tuply;

    //set the first element of the tuple, which is the sum of distances to current position +(n-1)*200
    //why? because this is the only existing entry, hence, the distance in other files to reach this
    //funtion signature is set to the max = MAX_POSSIBLE_INDEX
    tuply.sum = index + (numProcs-1)*( MAX_POSSIBLE_INDEX );

    //set the second element of the tuple to the vector of queues that we created
    tuply.indices = v;

    //insert the entry <functionSignature, tuple> into the hashtable
    my_hashmap[key] = tuply;
#ifdef PRINT_DEBUG
    cout << " " << tuply.sum << endl;
#endif
  }
  //if entry exists:
  else {
    //in this case we need to update that entry
    //push the current position into the corresponding queue
    Tuple &cur_entry = hm_it->second;

    cur_entry.indices[pid].push(index);
#ifdef PRINT_DEBUG
    cout << " " << cur_entry.sum << " --> ";
#endif

    //if this is the first entry for the process, after pushing the size of the queue will be one;
    //we need to update the sum by subtracting 200 and adding the current position.
    if(cur_entry.indices[pid].size()==1) {
      //updating sumOfMins
      cur_entry.sum += index - MAX_POSSIBLE_INDEX;
    }
#ifdef PRINT_DEBUG
    cout << cur_entry.sum << endl;
#endif
  }
}

/*remove the corresponding entry from our data structure and update the sumOfMins*/
void GlobalHash::remove(string &key, unsigned pid, unsigned index) {
#ifdef PRINT_DEBUG
  cout << "Removing .. " << key;
#endif

  //first get an iterator pointing to the hashtable entry we want to modify
  unordered_map<string, Tuple>::iterator hm_it;
  hm_it = my_hashmap.find(key);

  //queue<int> &cur_q = hm_it->second.second[pid];
  //int &cur_val = hm_it->second.
  Tuple &cur_entry = hm_it->second;

  //save the value of the removed entry in a temporary variable so that we can remove it from the sum
  int popped = cur_entry.indices[pid].front();

  //remove the entry by popping the corresponding queue
  cur_entry.indices[pid].pop();

  //check if size equal to zero before updating the sum
  //if it is equal to zero, then there are no more entries in the queue:
  //we need to add max_depth=200 that indicates that and subtract the value of the removed entry
#ifdef PRINT_DEBUG
  cout << " : " << cur_entry.sum << " --> " ;
#endif
  if(cur_entry.indices[pid].size() == 0)
    cur_entry.sum += (MAX_DEPTH - popped);
  else
    //we add the value of the current first smallest entry in the queue and subtract the value of the removed entry
    cur_entry.sum += (cur_entry.indices[pid].front() - popped);

#ifdef PRINT_DEBUG
  cout << cur_entry.sum << endl;
#endif
}

void print_debug(vector<unsigned> &p) {
  for (unsigned i = 0; i<logs.size(); i++)
    cout << p[i] << "\t";

  cout << endl;
  for (unsigned i=0;i<logs.size();i++) {
    if (p[i] != logs[i].size()) {
      assert( p[i] < logs[i].size() && "for safety purpose, but this should not happen");
      cout << logs[i][p[i]]->getKey() << endl;
    }
    else
      cout << "--------------" << endl;
  }
}

/* Merge the trace files of all the processes to a single file

   Pseudo code of what we want to do with hashmap:
   1. add the first 200 entries of each file to the data structure called my_hashmap.
   - my_hashmap is a hashtable indexed by the signature of the function (function name + parameters).
   - the second entry in the hashtable is a tuple where we store: the sum of the distances of the earliest occurences
   (i.e. the minimum distance for each file or process)
   - the second entry in the tuple is a queue where we keep the collisions: that way we always pop the smallest and
   insert the biggest at the end of the queue

   2. in the part where same=0:
   - to get the average distance for each entry, we simply access the hashmap, get that value. */

void work()
{
  unsigned n = logs.size();
  vector <unsigned> p (n, 0); // index of the first unprocessed entry (but in the global hash) of each proc
  vector <unsigned> q (n, 0); // index of the first entry not in the global hash of each proc
  vector<bool> merge(n, true);

  unsigned i;

  GlobalHash *gh = new GlobalHash(logs.size());

  for(i=0; i<n;i++) {
    for(unsigned inner_it=0; inner_it < MAX_DEPTH;inner_it++) {
      if(inner_it == logs[i].size())
        break;

      gh->insert(logs[i][inner_it]->getKey(), i, inner_it);
      q[i] = inner_it + 1;
    }
  }

  unsigned finished = 0;
  //Recycle the merging algorithm before all the trace files have been processed
  while (finished != n) {
#ifdef PRINT_DEBUG
    print_debug(p);
#endif

    // assume first all can be merged
    fill(merge.begin(), merge.end(), true);

        unsigned first = 0;
    while (p[first] == logs[first].size() && first < logs.size())
      first++;
    if (first == logs.size())
      break; // nothing to do...

    // do not consider record that is out of range
    for (i=0;i<n;i++) {
      if (p[i] == logs[i].size())
        merge[i] = false;
    }

    //If the current records of all the trace files can be merged, same=1; if not, same=0
    bool same = true;
    for (i=0; i<n; i++) {
      if (!merge[i]) continue;

      assert( p[i] < logs[i].size() && "for safety purpose, but this should not happen");
      if (*logs[i][p[i]] != *logs[first][p[first]]) {
        same = false;
        break;
      }
    }

    if (!same) {
      unsigned min_pos = p[0];

      double avg_fori, max_avg = -INF;
      int reci = -1;

      // need the min of all pos for normalization
      for (i=0; i<n; i++)
        if (p[i] < min_pos)
          min_pos = p[i];

      for (i=0; i<n; i++) {
        if (!merge[i]) continue;

        assert( p[i] < logs[i].size() && "for safety purpose, but this should not happen");
        double optval = gh->getOptVal(logs[i][p[i]]);
        avg_fori = (optval / n) - min_pos;
#ifdef PRINT_DEBUG
        cout << optval << "/" << avg_fori << " ";
#endif

        if (avg_fori > max_avg) {
          max_avg = avg_fori;
          reci = i;
        }
        else if (avg_fori == max_avg && reci != -1 && p[i] < p[reci])
          reci = i;
      }
#ifdef PRINT_DEBUG
      cout << " --> " << reci << endl;
#endif

      // update what to be merged with p[reci]
      for (i=0; i<n; i++) {
        if (!merge[i]) continue;
        assert( p[i] < logs[i].size() && "for safety purpose, but this should not happen");
        merge[i] = (*logs[i][p[i]] == *logs[reci][p[reci]]);
      }
    }

    // if all the same just show it
    showFunc(merge, p);

    for (i=0; i<n; i++) {
      if (merge[i]) {

        assert( p[i] < logs[i].size() && "for safety purpose, but this should not happen");
        gh->remove(logs[i][p[i]]->getKey(), i, p[i]);
        p[i]++;

        //insert a new entry instead of the one we removed
        if(q[i]!=logs[i].size()) {
          gh->insert(logs[i][q[i]]->getKey(), i, q[i]);
          q[i]++;
        }
      }
    }

    finished = 0;
    for (i=0; i<n; i++)
      finished += (logs[i].size() == p[i]);
#ifdef PRINT_DEBUG
    cout << endl;
#endif
  } // end while

  delete gh;
}

/*Save the records of the current file in line and save line in file*/
void readfile()
{
  string s;
  int start, end;

  getline(input_is, s);
  vector<LogEntry *> thislog;

  while (input_is) {
    start=s.find(":")+2;
    end=s.size();

    string entry = (s.substr(start, end - start + 1));
    thislog.push_back(new LogEntry(entry));

    getline(input_is, s);
  }
  logs.push_back(thislog);
}

/*Get the name of the next file*/
string nextName(string name)
{
  ostringstream n;

  int posi = name.find(".");
  int nextNumber = atoi(name.substr(posi+1, name.size()).c_str()) + 1;
  n << name.substr(0, posi).c_str() << "." << nextNumber;
  cout << n.str() << endl;
  return n.str();
}

int main(int argc,char *argv[])
{
  string nameIn;

  if (argc != 4) {
    cout << "Usage: combine inputfile outputfile $n" << endl << endl;
    return 0;
  }

  nameIn.assign(argv[1]);

  for (int i=0;i<atoi(argv[3]);i++) {
    input_is.open(nameIn.c_str());

    if (input_is.fail()) {
      cout << "Bad file name: [" << nameIn << "]!" << endl << endl;
      return 0;
    }

    cout << "Combine(V2.0)" << endl;
    cout << "Reading " << nameIn << "...";

    readfile();

    input_is.close();
    input_is.clear();

    cout << "Done." << endl;
    nameIn=nextName(nameIn);
  }

  res_os.open(argv[2]);
  work();

  res_os.close();
  res_os.clear();

  return 0;
}
