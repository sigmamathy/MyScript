#include <myscript/myscript.hpp>
#include <sstream>
#include <iostream>

namespace MyScript {

Config::Config()
{
}

void Config::Define(std::string const& name, FunctionInfo const& function)
{
	fnmap[name] = function;
}

struct SourceCompiler
{
	SourceCompiler(std::unordered_map<std::string, FunctionInfo> const& fnmap);
	void BeginCompile(std::string const& src);
	void SeparatorRead();
	void EndLineRead();

	template<class Ty>
	void TryInputNumber();
	void TryInputString();
	void TryInputBoolean();

	// sources and dependencies
	std::string source;
	std::unordered_map<std::string, FunctionInfo> const& fnmap;
	// results
	std::vector<std::function<void()>> instructions;

	// runtime temporary storage
	char prevc;
	std::string temp;
	unsigned line;
	FunctionInfo const* fntemp;
	int lookfor_n;
	Parameters paramstemp;
	bool string_read;

	// error flag
	bool error;
	std::string error_message;
};

#define REPORT_ERROR(err) { error = true, error_message = err; return; }

SourceCompiler::SourceCompiler(std::unordered_map<std::string, FunctionInfo> const& fnmap)
	: fnmap(fnmap)
{
	temp = "";
	prevc = '\0';
	line = 1u;
	lookfor_n = -1; // fncall
	string_read = false;
	error = false;
}

void SourceCompiler::BeginCompile(std::string const& src)
{
	source = src;
	for (int i = 0; i < source.length(); prevc = source[i++])
	{
		char c = source[i];
		if (c == '"') string_read = !string_read;

		if (c == '\n') EndLineRead();
		else if ((c == ' ' || c == ',' || c == '\t') && !string_read) SeparatorRead();
		else temp += c;

		if (error) {
			return;
		}
	}
	// complete reading
	EndLineRead();
}

void SourceCompiler::SeparatorRead()
{
	// no read required
	if (prevc == '\0' || prevc == '\n' || prevc == ' ' || prevc == ',' || prevc == '\t')
		return;

	if (lookfor_n == -1) { // fncall
		if (!fnmap.count(temp))
			REPORT_ERROR("Attempting to access unknown function reference: \"" + temp + "\" does not exists");
		fntemp = &fnmap.at(temp);
	}

	else { // param
		if (lookfor_n == fntemp->params.size()) // over the range
			REPORT_ERROR("Illegal number of arguments.");
		switch (fntemp->params[lookfor_n])
		{
#define MATCH_CASE_NUM(n, ty) case n: TryInputNumber<ty>(); break
			using enum ParamType;
			MATCH_CASE_NUM(i32, std::int32_t);
			MATCH_CASE_NUM(u32, std::uint32_t);
			MATCH_CASE_NUM(i64, std::int64_t);
			MATCH_CASE_NUM(u64, std::uint64_t);
			MATCH_CASE_NUM(f32, float);
			MATCH_CASE_NUM(f64, double);
#undef MATCH_CASE_NUM
		case str:
			TryInputString();
			break;
		case bl:
			TryInputBoolean();
			break;
		}
		if (error) {
			return;
		}
	}

	++lookfor_n;
	temp = "";
}

void SourceCompiler::EndLineRead()
{
	bool nsepread = prevc != ' ' && prevc != ',' && prevc != '\t';
	// Escape if the line is garbage
	if (prevc == '\0' || prevc == '\n' || (!nsepread && lookfor_n == -1))
		return;

	if (string_read)
		REPORT_ERROR("Strings not enclosed within line")

	// check the last parameter anyways
	if (nsepread) {
		if (lookfor_n != fntemp->params.size() - 1) // not exactly equal to the range
			REPORT_ERROR("Illegal number of arguments.");
		if (lookfor_n == -1) { // fncall
			if (!fnmap.count(temp))
				REPORT_ERROR("Attempting to access unknown function reference: \"" + temp + "\" does not exists");
			fntemp = &fnmap.at(temp);
		}
		else switch (fntemp->params[lookfor_n]) // params
		{
#define MATCH_CASE_NUM(n, ty) case n: TryInputNumber<ty>(); break
			using enum ParamType;
			MATCH_CASE_NUM(i32, std::int32_t);
			MATCH_CASE_NUM(u32, std::uint32_t);
			MATCH_CASE_NUM(i64, std::int64_t);
			MATCH_CASE_NUM(u64, std::uint64_t);
			MATCH_CASE_NUM(f32, float);
			MATCH_CASE_NUM(f64, double);
#undef MATCH_CASE_NUM
		case str:
			TryInputString();
			break;
		case bl:
			TryInputBoolean();
			break;
		}
		if (error) {
			return;
		}
	}
	else if (lookfor_n != fntemp->params.size()) { // should be exactly one offset to the range
		REPORT_ERROR("Illegal number of arguments.");
	}

	// setup new instruction
	auto ptr = fntemp->fnptr;
	auto params = paramstemp;
	instructions.emplace_back([=]() { ptr(params); });

	// reset state
	fntemp = nullptr;
	lookfor_n = -1;
	paramstemp.clear();
	temp = "";
	line++;
}

template<class Ty>
void SourceCompiler::TryInputNumber()
{
	if (std::is_unsigned_v<Ty> && temp[0] == '-')
		REPORT_ERROR("Type mismatch");
	std::stringstream ss;
	ss << temp;
	Ty x;
	ss >> x;
	if (!ss.eof() || ss.fail()) REPORT_ERROR("Type mismatch");
	paramstemp.emplace_back(x);
}

void SourceCompiler::TryInputString()
{
	if (temp.length() < 2 || temp[0] != '"' || temp.back() != '"')
		REPORT_ERROR("Type mismatch");
	paramstemp.emplace_back(temp.substr(1, temp.length() - 2));
}

void SourceCompiler::TryInputBoolean()
{
	if (temp == "true" || temp == "yes" || temp == "on") { paramstemp.emplace_back(true); return; }
	if (temp == "false" || temp == "no" || temp == "off") { paramstemp.emplace_back(false); return; }
	REPORT_ERROR("Type mismatch");
}

Executable Config::CompileSource(std::string const& source)
{
	SourceCompiler compiler(fnmap);
	compiler.BeginCompile(source);
	if (compiler.error) {
		std::cout << compiler.error_message << '\n';
		return []{};
	}

	return [compiler]() -> void {
		for (auto& task : compiler.instructions)
			task();
	};
}

} // !MyScript