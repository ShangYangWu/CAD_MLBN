#include "kernel.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <queue>
#include <unordered_set>

using namespace std;

void Kernel::parse(const vector<TwoLevelNode> &all_nodes, const unordered_map<string, vector<string>> &primary_input, const vector<string> &primary_output, const size_t &literal_count, const string &model_name, const vector<string> origin_inputs){
  PrimaryInputs = primary_input; 
  PrimaryOutputs = primary_output; 
  OriginInputs = origin_inputs;
  LitCount = literal_count;
  ModelName = model_name;


  for (const auto &node : all_nodes) {
    Mapping[node.output] = node;
  }
}

int Kernel::getKernel(void){
    
    size_t kernel_count = 1;
    // vector<vector<string>> ret;

    for(auto &term : PrimaryOutputs){

        // ternimal一樣的分一起做一個kernel
        vector<string> same_kernel;
        for(auto &nodes : Mapping){
            if(nodes.second.terminal == term){
                // cout << nodes.second.output;
                same_kernel.emplace_back(nodes.second.output);
            }
        }
// 檢查: 正在哪一個output
        // cout << term << endl;

        // 第一步 找到所有的 cof(eg. a, b), kernel成員(eg. o1 o2) *kernel成員一樣的去除
        for (auto i = PrimaryInputs[term].begin(); i != PrimaryInputs[term].end(); ++i) {
            auto &element = *i;
            vector<string> same_cof_nodes; // 存o1 o2 o3
            set<string> cof; // 存 cofactor = kernel.cofactor

            for (const auto& item : same_kernel) { // o1 o2 o3 o4 o5
                const vector<string>& input = Mapping[item].inputs; // o1 o2 o3 o4 o5 的 product
                if (find(input.begin(), input.end(), element) != input.end()) { // 若 a 是 o1 的 product
                    same_cof_nodes.emplace_back(item); // o1 存進 same_cof_nodes
                }
            }
            if (same_cof_nodes.size() <= 1) { // same_cof_nodes 中只有 o1
                continue; // cof 不存 a
            }else{ // same_cof_nodes 中有 o1 o2 o3
                cof.insert(element);  // cof 存 a
            }
// 檢查: 輸出"哪一些node有包含element"
            // cout << "Element: " << element << " Found in: ";
            // for (const string& node : same_cof_nodes) {
            //     cout << node << " ";
            // }
            // cout << endl;

            for (auto j = PrimaryInputs[term].begin(); j != PrimaryInputs[term].end(); ++j) {// 已經存 a 檢查 b c d 是不是也是cof
                auto &element2 = *j;
                vector<string> temp_same_cof_nodes; // 紀錄 b 是不是 o1 o2 o3 的 cofactor
                for (const auto& item : same_cof_nodes) { // o1 o2 o3
                    const vector<string>& input = Mapping[item].inputs; // o1 o2 o3 的 product
                    if (find(input.begin(), input.end(), element2) != input.end()) { // 若 b 是 o1 的 product
                        temp_same_cof_nodes.emplace_back(item);
                    }
                }
                if (temp_same_cof_nodes.size() == same_cof_nodes.size()) { // o1 o2 o3 有 a 也有 b
                    cof.insert(element2); // cof 存 a b
                }
            }
            // 記錄一筆kernel
            bool kernel_exist = 0;
            for(auto &ker : TwoLevelKernels){
                if(ker.cofactor == cof){
                    kernel_exist = 1;
                }                    
            }
            if(kernel_exist == 0){
                twolevel_kernel kernel; // 宣告一筆kernel
                kernel.kernel_name = term; // 紀錄一筆kernel_name
                kernel_count++;
                for(auto &item : cof){
                    kernel.cofactor.insert(item); // 記錄一筆cofactor
                }

                for (const string& node : same_cof_nodes) { // kernel成員(eg. o1)去除cofactor之後的元素
                    const vector<string>& vec = Mapping[node].inputs; // o1 的 product
                    vector<string> remainingElements; // 存 o1 去除 cofactor 之後的元素
                    set_difference(vec.begin(), vec.end(), kernel.cofactor.begin(), kernel.cofactor.end(), back_inserter(remainingElements)); // 存 o1 去除 cofactor 之後的元素
                
                    string kernelproduct;
                    for (const string& elem : remainingElements) { // 遍歷 o1 去除 cofactor 之後的元素
                        kernelproduct = kernelproduct + elem; // 將剩餘元素重新命名
                    }
                    kernel.outputs.insert(node);
                    kernel.kernel_product[node] = remainingElements; // 紀錄 o1除以cofactor 後的元素與重新命名後的對照

// 檢查: 剩餘元素
                    // cout << " - Remaining Elements in " << node << " after removing ";
                    // for(auto &it : cof){
                    //     cout << it << " ";
                    // }
                    // cout << ": ";
                    // for (const string& elem : remainingElements) {
                    //     cout <<elem << " ";
                    // }
                    // cout << endl;
                }
                TwoLevelKernels.emplace_back(kernel);
            }
        }
    }

// 檢查: TwoLevelKernels中所有資訊
    // for(auto &item: TwoLevelKernels){
    //     cout << item.kernel_name << " : " << endl;
    //     for(auto &it : item.outputs){
    //         cout << it <<" ";
    //     }
    //     cout << "\n - cofactor :";
    //     for(auto &it : item.cofactor){
    //         cout << it <<" ";
    //     }
    //     cout << "\n - kernel_product :";
    //     for(auto &it : item.kernel_product){
    //         cout << it.first << " ";
    //     }
    //     cout << endl;
    // }

    // 第二步 同 terminal 的 SOPs/cofactor
    size_t final_literal = 0;
    vector<twolevel_kernel> save_list;
    for(auto &term : PrimaryOutputs){

        // ternimal一樣的做成set
        size_t min_literal = 0;
        set<string> same_term;
        for(auto &nodes : Mapping){
            if(nodes.second.terminal == term){
                // cout << nodes.second.output;
                same_term.insert(nodes.second.output);
                min_literal = min_literal + nodes.second.inputs.size();
            }
        }
        // 嘗試
        twolevel_kernel save;
        size_t literal_count = 0;
        for(auto &twolevelkernel : TwoLevelKernels){
            set<string> tmp_same_term = same_term;
            literal_count = 0;
            if(twolevelkernel.kernel_name == term && twolevelkernel.cofactor.size()>1){
                
                for(auto &node : twolevelkernel.kernel_product){
                    literal_count+=node.second.size();
                    // for(auto &input: node.second){
                    //     cout << input << " ";
                    // }
                    // cout << "-> "<< node.first << " " << term << "_new" << endl;
                    tmp_same_term.erase(node.first);
                }

                literal_count += twolevelkernel.cofactor.size();
                literal_count++;
                // for(auto &cof : twolevelkernel.cofactor){
                //     cout << cof << " ";
                // }
                // cout << term << "_new" << endl;

                for(auto &node : tmp_same_term){
                    literal_count+=Mapping[node].inputs.size();
                    literal_count++;
                    // for(auto &input : Mapping[node].inputs){
                    //     cout << input << " ";
                    // }
                    // cout << term << "_new " << "->" << Mapping[node].output << endl;
                }
                // cout << literal_count << endl;
                if(min_literal >= literal_count){
                    min_literal = literal_count;
                    save = twolevelkernel;
                }
            }
        }
        save_list.emplace_back(save);
        final_literal += min_literal;
// 檢查: save是否正確
        // if(save.cofactor.size()>0){
        //     for(auto &cof : save.cofactor){
        //         cout << cof << " ";
        //     }
        //     cout << "-> " << term << "_new" << endl;
        // }
        // for(auto &node : save.kernel_product){
        //     cout << term << "_new ";
        //     for(auto &input: node.second){
        //         cout << input << " ";
        //     }
        //     cout << "-> "<< node.first << endl;
        //     same_term.erase(node.first);
        // }
        // for(auto &node : same_term){
        //     for(auto &input : Mapping[node].inputs){
        //         cout << input << " ";
        //     }
        //     cout << "->" << Mapping[node].output << endl;
        // }
    }
    cout << "Orignal literal count: " << LitCount << endl; 
    cout << "Optimized literal count: " << final_literal << endl;

    // 寫成檔案

    ofstream outFile("out.blif");

    if (outFile.is_open()) {
        streambuf *coutbuf = cout.rdbuf();
        cout.rdbuf(outFile.rdbuf());


        cout << ".model " << ModelName << endl;
        cout << ".inputs ";
        for(auto &input: OriginInputs){
            cout << input << " ";
        }
        cout << endl;
        cout << ".outputs ";
        for(auto &output: PrimaryOutputs){
            cout << output << " ";
        }
        cout << endl;

        size_t count = 0;

        set<string> cofless_term;
        for(auto &term : PrimaryOutputs){
            cofless_term.insert(term);
        }

        for(auto &term : PrimaryOutputs){
            set<string> same_term;
            for(auto &nodes : Mapping){
                if(nodes.second.terminal == term){
                    // cout << nodes.second.output;
                    same_term.insert(nodes.second.output);
                }
            }

            for(auto &save : save_list){
                cofless_term.erase(save.kernel_name);
                if(save.kernel_name == term){
                    set<string> tmp_inputs;

                    if(save.cofactor.size()>0){

                        for(auto &node : save.kernel_product){
                            for(auto &input: node.second){
                                for(char ch : input){
                                    if (ch != '\'') {
                                        tmp_inputs.insert(string(1, ch));
                                    }                                
                                }
                            }
                        }
                        
                        cout << ".names ";
                        for(auto &input : tmp_inputs){
                            cout << input << " ";
                        }
                        cout << save.kernel_name << "_" << count << endl;

                        for(auto &node : save.kernel_product){
                            set<string> unsigned_node;
                            for(auto &input: node.second){
                                for(char ch : input){
                                    if(ch != '\''){
                                        unsigned_node.insert(string(1, ch));
                                    }                    
                                }
                            }                        

                            for(auto t = tmp_inputs.begin(); t != tmp_inputs.end(); ++t){
                                auto &input = *t;                         
                                auto it = unsigned_node.find(input);
                                auto inv = find(node.second.begin(), node.second.end(), input);
                                if (it != unsigned_node.end() && inv != node.second.end()) {
                                    cout << "1";
                                } else if (it != unsigned_node.end() && inv == node.second.end()) {
                                    cout << "0";
                                } else {
                                    cout << "-";
                                }
                            }
                            cout << " 1" << endl;
                            same_term.erase(node.first);       
                        }
                    }

                    set<string> final_inputs;
                    final_inputs.insert(save.kernel_name + "_" + to_string(count));
                    if(save.cofactor.size()>0){
                        for(const auto &cof : save.cofactor){
                            for (char ch : cof) {
                                if (ch != '\'') {
                                    final_inputs.insert(string(1, ch));
                                }
                            }                            
                        }                                        
                    }
                    for(auto &node : same_term){
                        for(auto &input : Mapping[node].inputs){
                            for (char ch : input) {
                                if (ch != '\'') {
                                    final_inputs.insert(string(1, ch));
                                }
                            } 
                        }
                    }
                    cout << ".names ";
                    for(const auto &final_input : final_inputs){
                        cout << final_input << " ";                         
                    }
                    cout << term << endl;

                        
                    for(auto &node : same_term){
                        set<string> unsigned_node;
                        for(auto &input : Mapping[node].inputs){
                            for(char ch : input){
                                if(ch != '\''){
                                    unsigned_node.insert(string(1, ch));
                                }                    
                            }
                        }
                    
                        for(auto t = final_inputs.begin(); t != final_inputs.end(); ++t){
                            auto &input = *t;                         
                            auto it = unsigned_node.find(input);
                            auto inv = find(Mapping[node].inputs.begin(), Mapping[node].inputs.end(), input);
                            if (it != unsigned_node.end() && inv != Mapping[node].inputs.end()) {
                                cout << "1";
                            } else if (it != unsigned_node.end() && inv == Mapping[node].inputs.end()) {
                                cout << "0";
                            } else if( input == term+"_0" ){
                                cout << "0";
                            } else {
                                cout << "-";
                            }
                        } 
                        cout << " 1" << endl;
                    }

                    if(save.cofactor.size()>0){
                        set<string> unsigned_node;
                        for(const auto &cof : save.cofactor){
                            for (char ch : cof) {
                                if (ch != '\'') {
                                    unsigned_node.insert(string(1, ch));
                                }
                            }     
                        }                       
                        for(auto t = final_inputs.begin(); t != final_inputs.end(); ++t){
                            auto &input = *t;                         
                            auto it = unsigned_node.find(input);
                            auto inv = find(save.cofactor.begin(), save.cofactor.end(), input);
                            if (it != unsigned_node.end() && inv != save.cofactor.end()) {
                                cout << "1";
                            } else if (it != unsigned_node.end() && inv == save.cofactor.end()) {
                                cout << "0";
                            } else if( input == term+"_0" ){
                                cout << "1";
                            } else {
                                cout << "-";
                            }
                        } 
                        cout << " 1" << endl;                                          
                    }
                }          
            }
        }
        
        for(auto &term : cofless_term){
            cout << ".names " ;

            set<string> unsigned_inputs;
            vector<TwoLevelNode> same_term;
            for(auto &node : Mapping){
                if(node.second.terminal == term){
                    same_term.emplace_back(node.second);
                    for(auto &input : node.second.inputs){
                        for(auto ch : input){
                            if( ch != '\''){
                                unsigned_inputs.insert(string(1, ch));
                            }
                        }
                    }
                }
            }
            for(auto &input : unsigned_inputs){
                cout << input << " ";
            }
            cout << term << endl;
            
            for(auto &node : same_term){
                set<string> unsigned_node;
                for(auto &input : node.inputs){
                    for(auto ch : input){
                        if( ch != '\''){
                            unsigned_node.insert(string(1, ch));
                        }
                    }
                }
                for(auto &input : unsigned_inputs){
                    auto it = unsigned_node.find(input);
                    auto inv = find(node.inputs.begin(), node.inputs.end(), input);
                    if (it != unsigned_node.end() && inv != node.inputs.end()) {
                        cout << "1";
                    } else if (it != unsigned_node.end() && inv == node.inputs.end()) {
                        cout << "0";
                    } else {
                        cout << "-";
                    }                    
                }
                cout << " 1" << endl;                
            }
        }
               
        cout << ".end" << endl;

        // 還原 cout 到原始狀態
        cout.rdbuf(coutbuf);

        outFile.close(); // 關閉文件

    } else {
        cerr << "無法打開文件!" << endl;
    }
    return 0;
}
