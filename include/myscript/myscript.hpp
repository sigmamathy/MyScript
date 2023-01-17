#pragma once

#include <string>
#include <variant>
#include <functional>
#include <unordered_map>

namespace MyScript {

// Describes function parameters
enum class ParamType {
	i32, u32, i64, u64, f32, f64, str, bl
};

// Common variance
using CommonType = std::variant<
	std::int32_t,		// i32
	std::uint32_t,		// u32
	std::int64_t,		// i64
	std::uint64_t,		// u64
	float,				// f32
	double,				// f64
	std::string,		// str
	bool				// bl
>;

using Parameters = std::vector<CommonType>;

// Local script functions
struct FunctionInfo {
	// function pointer that takes "MyScript::Parameters" as parameter
	std::function<void(Parameters const&)> fnptr;
	// state the parameters required, refer to "MyScript::ParamType"
	std::vector<ParamType> params;
};

// Executable objects
using Executable = std::function<void()>;

// Initial configuration
class Config
{
public:
	//  Initialize the configuration.
	Config();

	// Defines a function for the script to use.
	// Notice that the name should only includes alphabets, other characters might cause undefined behaviour.
	void Define(std::string const& name, FunctionInfo const& function);

	// Same functionality as above, but directly construct inside here.
	template<ParamType... types>
	void Define(std::string const& name, std::function<void(Parameters const&)> const& fnptr) {
		FunctionInfo info = { fnptr, {types...} };
		Define(name, info);
	}

	// Compile the source given into an executable functor, such functor can be be called later.
	Executable CompileSource(std::string const& source);
	
private:
	std::unordered_map<std::string, FunctionInfo> fnmap;
};

} // !MyScript