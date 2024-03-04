# pragma once

# include "parser.hpp"
# include <set>

using namespace std;

struct twolevel_kernel{
    string kernel_name;
    set<string> cofactor;
    unordered_map<string, vector<string>> kernel_product;
    set<string> outputs;
};

struct Kernel{
    // 紀錄 two level
    unordered_map<string, TwoLevelNode> Mapping;
    unordered_map<string, vector<string>> PrimaryInputs;
    vector<string> PrimaryOutputs;
    vector<string> OriginInputs;
    vector<twolevel_kernel> TwoLevelKernels;
    // 紀錄 multi level
    set<string> TwolevelInputs;
    vector<string> OrderedTwolevelInputs;
    size_t LitCount;
    string ModelName;

    void parse(const vector<TwoLevelNode> &all_nodes, const unordered_map<string, vector<string>> &primary_input, const vector<string> &primary_output, const size_t &literal_count, const string &model_name, const vector<string> origin_inputs);

    // vector<twolevel_kernel> getAllTwoLevelKernels(void) { return TwoLevelKernels; }

    // vector<vector<string>> getKernel(void);
    int getKernel(void);
};


