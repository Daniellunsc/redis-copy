#include <unordered_map>
#include <string>
#include <iostream>

namespace Config
{
  std::unordered_map<std::string, std::string> config;

  void set_config(std::string parameter, std::string value)
  {
    std::cout << "Setting config " << parameter << " to " << value << std::endl;
    config[parameter] = value;
  }

  std::string get_config(std::string parameter)
  {
    return config[parameter];
  }

  void map_arg_to_config(int argc, char **argv)
  {
    for (int i = 0; i < argc; i++)
    {
      if (std::string("--dir").compare(argv[i]) == 0)
      {
        set_config("dir", argv[i + 1]);
      }
      if (std::string("--dbfilename").compare(argv[i]) == 0)
      {
        set_config("dbfilename", argv[i + 1]);
      }
    }
  }
}
