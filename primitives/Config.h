#ifndef CONFIG_INCLUDE_H
#define CONFIG_INCLUDE_H

#include <string>
#include <mutex>

/**
 * Singleton that handles configuration settings.
 */
class Config
{
public:
    /**
     * Get instance of singleton
     * @return Instance
     */
    static Config &getInstance();
    
    const int getConfigVersion() const;

    /**
     * Get data of an option
     * @param name The name of the option that should be returned
     * @return Option data
     */
    std::string getOption(const std::string &name) const;

    /**
     * Set new data for an option
     * @param name The name of the option
     * @param option New data for the option
     */
    void setOption(const std::string& name, const std::string& option);
    
private:
    static const std::string SETTINGS_FILE;

	mutable std::mutex m_mutex;
    std::string m_file_name;

    /**
     * Read the settings file.
     * If settings file doesn't exist it creates one and fills it with 
     * default data
     */
    Config();
    Config(const Config &original);
    virtual ~Config();

    /**
     * Set default configuration settings
     */
    void setDefaults();
};

#endif // CONFIG_INCLUDE_H
