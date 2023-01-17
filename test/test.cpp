#include <myscript/myscript.hpp>
#include <iostream>

void MyFavouriteFunction(MyScript::Parameters const& params)
{
	auto name = std::get<std::string>(params[0]);
	int age = std::get<std::int32_t>(params[1]);
	std::cout << "Person: name = " << name << ", age = " << age << '\n';
}

static std::string script = R"(
DisplayPerson "Joseph" -20
DisplayPerson "Pasley Ha" 14)";

int main()
{
	MyScript::Config config;
	using enum MyScript::ParamType;
	config.Define<str, i32>("DisplayPerson", MyFavouriteFunction);
	auto exec = config.CompileSource(script);
	exec();
	
	return 0;
}