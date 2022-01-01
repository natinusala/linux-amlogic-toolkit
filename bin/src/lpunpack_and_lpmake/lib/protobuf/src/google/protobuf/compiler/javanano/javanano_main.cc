#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/compiler/javanano/javanano_generator.h>
#include <google/protobuf/compiler/plugin.h>

int main(int argc, char* argv[]) {
  google::protobuf::compiler::javanano::JavaNanoGenerator generator;
  return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}
