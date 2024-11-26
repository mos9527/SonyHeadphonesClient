#include <string>
void ExecuteShellCommand(const std::string& command) {
  system(command.c_str());
}