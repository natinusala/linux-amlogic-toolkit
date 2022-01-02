#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/compiler/javamicro/javamicro_generator.h>
#include <google/protobuf/compiler/plugin.h>

int main(int argc, char* argv[]) {
  google::protobuf::compiler::javamicro::JavaMicroGenerator generator;
  return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}
