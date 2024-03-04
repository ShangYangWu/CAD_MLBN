#pragma once

#include <iostream>
#include <string>
using namespace std;

struct ArgParser {
  string file;
};

inline void PrintError(int argc, char **argv) {
  cout << "Command: ";
  for (int i = 0; i < argc; ++i) {
    cout << argv[i] << " ";
  }
  cout << "is not a legal command.\n";
  cout << "Please use: \n";
  cout << "  " << argv[0] << " <blif>\n";
  exit(1);
}

inline ArgParser ParseArgument(int argc, char **argv) {
  if (argc != 2) {
    PrintError(argc, argv);
  }
  ArgParser p;
  p.file = argv[1];

  return p;
}