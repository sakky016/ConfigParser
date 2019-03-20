#ifndef _CONFIG_PARSER_H
#define _CONFIG_PARSER_H
#include <fstream>
#include<iostream>
#include<stdio.h>
#include<string>
#include<unordered_map>
#include<map>

using namespace std;

//------------------------------------------------------------------------------------------------
// Globals
//------------------------------------------------------------------------------------------------
const char COMMENT_CHAR = '#';                  // Specifies which character will be used for commenting
const char KEY_VALUE_SEPARATOR = '=';           // Specifies delimiter for key/value pair

//------------------------------------------------------------------------------------------------
// Class for Configuration Parser
//------------------------------------------------------------------------------------------------
class ConfigParser
{
private:
    bool                                    m_debug;
    string                                  m_configFileName;
    ifstream                                m_fileStream;
    unordered_map<string, string>           m_configMap;
    map<int, string>                        m_invalidLines;
    long int                                m_lines;
    long int                                m_ignoreLines;
    long int                                m_validEntries;
    long int                                m_invalidEntries;

public:
    ConfigParser(bool debug, const string & filename);
    ~ConfigParser();

    bool isDebugEnabled() { return m_debug; }
    unordered_map<string, string> ParseConfigFile();
    long GetFileSize();
    bool SplitLineToKeyValue(const string & line, string & key, string & value);
    void DisplayParsingDetails();
    void DisplayInvalidLines();
};

#endif
