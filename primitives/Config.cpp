#include "Config.h"

#include <fstream>
#include <iomanip>

#include "nlohmann/json.hpp"

using json = nlohmann::json;


#ifndef WIN32
const std::string Config::SETTINGS_FILE = "/var/lib/SphinxDetectors/settings.json";
#else
const std::string Config::SETTINGS_FILE = "settings.json";
#endif


bool is_file_exist(const char *fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

//-----------------------------------------------------------------------------
Config::Config() :
    m_file_name(SETTINGS_FILE)
    
{
    if (!is_file_exist(m_file_name.c_str()) || getConfigVersion() != 10)
	{
        setDefaults();
    }
}

//-----------------------------------------------------------------------------
Config::~Config()
{
}

//-----------------------------------------------------------------------------
void Config::setDefaults()
{
	auto config = R"(
		{
			"configVer": "10",
			"port": "8080",
			"authrealm": "WEB SERVER",
			"userid": "admin",
			"hash": "YWRtaW4="
		}
	)"_json;

	std::ofstream o(m_file_name);
	o << std::setw(4) << config << std::endl;

}

//-----------------------------------------------------------------------------
Config &Config::getInstance()
{
    static Config instance;
    return instance;
}


const int Config::getConfigVersion() const
{
	return std::stoi(getOption("configVer"));
}

std::string Config::getOption(const std::string &name) const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	json config;

	std::ifstream i(m_file_name);
	i >> config;
	
	return  config.contains(name) ? config[name].get<std::string>() : std::string();
}

void Config::setOption(const std::string& name, const std::string& option)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	json config;
	{
		std::ifstream i(m_file_name);
		i >> config;
	}

	config[name] = option;

	std::ofstream o(m_file_name);
	o << std::setw(4) << config << std::endl;
}

