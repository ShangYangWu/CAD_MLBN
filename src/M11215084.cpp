
#include "arg_parser.hpp"
#include "parser.hpp"
#include "kernel.hpp"

using namespace std;

int main(int argc, char **argv) {

  auto arg = ParseArgument(argc, argv);

  Parser parse;
  parse.parse(arg.file);
  // parse.print();

  Kernel algorithm;
  algorithm.parse(parse.getAllNodes(), parse.inputs, parse.outputs, parse.literal_count, parse.ModelName, parse.origin_inputs);
  algorithm.getKernel();
}