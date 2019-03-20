#include"config_parser.h"

//------------------------------------------------------------------------------------------------
// Globals
//------------------------------------------------------------------------------------------------
const bool DEBUG_ENABLED = true;
const string CONFIG_FILENAME = "sample_input.cfg";


//------------------------------------------------------------------------------------------------
// Main for test Configuration Parser
//------------------------------------------------------------------------------------------------
int main()
{
    ConfigParser parser(DEBUG_ENABLED, CONFIG_FILENAME);
    unordered_map<string, string> configMap = parser.ParseConfigFile();

    getchar();
    return 0;
}