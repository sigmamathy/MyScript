#include <myscript/myscript.hpp>

namespace MyScript {

Config::Config()
{
}

void Config::Define(std::string const& name, FunctionInfo const& function)
{
	fnmap[name] = function;
}



}