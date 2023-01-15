#pragma once

#include <string>
#include <variant>
#include <functional>
#include <unordered_map>

namespace MyScript {

// Describes the function parameters
enum class ParamType {
	i32, ui32, f32, f64, str, bl
};

// Common variance
using CommonType = std::variant<
	std::int32_t,
	std::uint32_t,
	float,
	double,
	std::string,
	bool
>;

// Local script functions
struct FunctionInfo {
	std::function<std::vector<CommonType> const&> fnptr;
	// parameters required
	std::vector<ParamType> params;
};

// Initial configuration
class Config
{
public:
	Config();

	// Defines a function for the script to use.
	void Define(std::string const& name, FunctionInfo const& function);

	
private:
	std::unordered_map<std::string, FunctionInfo> fnmap;
};


} // MyScript