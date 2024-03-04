#pragma once

#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

struct TwoLevelNode {
  string output;
  vector<string> inputs;
  string terminal;

  // void print() const;
};

struct Parser {
  string ModelName;
  size_t literal_count;
  unordered_map<string, vector<string>> inputs;
  vector<string> outputs;
  vector<string> origin_inputs;
  vector<TwoLevelNode> nodes;

  void parse(const string &file);
  // void print() const;
  vector<TwoLevelNode> getAllNodes() { return nodes; }
};
