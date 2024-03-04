#include "parser.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>

using namespace std;

namespace {
  vector<string> split(const string &line) {
    vector<string> split;
    stringstream ss;
    ss << line;
    string s;
    while (ss >> s) {
      // if(s != "\\"){
        split.emplace_back(move(s));
      // }
    }
    return split;
  }
} // namespace


void Parser::parse(const string &file) {
  ifstream fin(file, ios::in);
  string buffer;
  TwoLevelNode node;
  string terminal_node;
  vector<string> temp_inputs;
  size_t product_count=0;
  literal_count=0;
  bool input_flag = 0;
  bool output_flag = 0;
  bool names_flag = 0;

  while (getline(fin, buffer)) {
    auto split_string = split(buffer);
    auto &title = split_string[0];
    auto &endding = split_string.back();

    if (title == ".model") {
        ModelName = split_string[1];
    } else if (title == ".inputs" || input_flag == 1){
      input_flag = 1;
      if(title == ".inputs" && endding == "\\"){
        move(split_string.begin()+1, split_string.end()-1, back_inserter(origin_inputs));
        continue;
      } else if(endding == "\\"){
        move(split_string.begin(), split_string.end()-1, back_inserter(origin_inputs));
        continue;
      } else {
        if(title == ".inputs"){
          move(split_string.begin() + 1, split_string.end(), back_inserter(origin_inputs));
        } else {    
          move(split_string.begin(), split_string.end(), back_inserter(origin_inputs));
        }
        input_flag = 0;
      }
    } else if (title == ".outputs" || output_flag == 1){
      output_flag = 1;
      if(title == ".outputs" && endding == "\\"){
        move(split_string.begin()+1, split_string.end()-1, back_inserter(outputs));
        continue;
      }else if(endding == "\\"){
        move(split_string.begin(), split_string.end()-1, back_inserter(outputs));
        continue;
      }else{
        if(title == ".outputs"){
          move(split_string.begin() + 1, split_string.end(), back_inserter(outputs));
        } else {    
          move(split_string.begin(), split_string.end(), back_inserter(outputs));
        }
        output_flag = 0;
      }
    } else if (title == ".names" || names_flag == 1){
      if(names_flag == 0){
        temp_inputs.clear();
      }
      names_flag = 1;
      product_count = 0;
      if(title == ".names" && endding == "\\"){
        move(split_string.begin() + 1, split_string.end() - 1, back_inserter(temp_inputs));
        continue;
      } else if(endding == "\\"){
        move(split_string.begin(), split_string.end() - 1, back_inserter(temp_inputs));
        continue;        
      } else {
        if(title == ".names"){
          move(split_string.begin() + 1, split_string.end() - 1, back_inserter(temp_inputs));
        }else{
          move(split_string.begin(), split_string.end() - 1, back_inserter(temp_inputs));
        }
        terminal_node = split_string.back();
        vector<string> input_vec;
        for (auto &it : temp_inputs) {
            input_vec.emplace_back(it);
            input_vec.emplace_back(it+"'");
        }
        inputs[terminal_node] = input_vec; 
        names_flag = 0;
      }
    } else if(title == ".end"){
      if (!node.output.empty()) {
          nodes.emplace_back(move(node));
          node.output.clear();
          node.inputs.clear();
      }
      return;
    } else {
      if (!node.output.empty()) {
        nodes.emplace_back(move(node));
        node.output.clear();
        node.inputs.clear();
      }
      product_count++;
      int idx = 0;
      for(char &n : split_string[0]){
        string logic(1, n);
        if(logic == "0"){
          node.inputs.emplace_back(temp_inputs[idx]+"'");
          literal_count++;
        }else if(logic == "1"){
          node.inputs.emplace_back(temp_inputs[idx]);
          literal_count++;
        } 
        idx++;         
      }
      node.output = terminal_node + "_" +to_string(product_count);
      node.terminal = terminal_node;
    }
  }
}

// void TwoLevelNode::print() const {

//   for (const auto &input : inputs) {
//     cout << input << " ";
//   }
//   cout << " -> ";
//   cout << output;
//   cout << " -> ";
//   cout << terminal << "\n";
// }


// void Parser::print() const {
//   cout << "Model: " << ModelName << "\n";
//   cout << "Inputs: " << endl;
//   for (const auto &input : inputs) {
//     cout << input.first << " : ";
//     for(auto &item : input.second){
//       cout << item << " ";
//     }
//     cout << endl; 
//   }
//   cout << "Outputs: ";
//   for (const auto &output : outputs) {
//     cout << output << " ";
//   }
//   cout << "\n";
//   cout << "ALL Logic:\n";
//   for (const auto &twolevelnode : nodes) {
//     twolevelnode.print();
//   }
//   cout << "Literal count: " << literal_count << endl;
// }