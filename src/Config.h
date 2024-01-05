#include <string>
#ifndef CONFIG_H
#define CONFIG_H

namespace Config{
  void map_arg_to_config(int argc, char **argv);
  void set_config(std::string parameter, std::string value);
  std::string get_config(std::string parameter);
}

#endif
