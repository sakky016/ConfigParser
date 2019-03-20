#include <cctype>           // for isspace
#include "config_parser.h"


//------------------------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------------------------
// Left trim the given string ("  hello!  " --> "hello!  ")
string left_trim(string str)
{
    int numStartSpaces = 0;
    for (int i = 0; i < str.length(); i++) 
    {
        if (!isspace(str[i])) 
            break;
        
        numStartSpaces++;
    }

    return str.substr(numStartSpaces);
}

// Right trim the given string ("  hello!  " --> "  hello!")
string right_trim(string str) 
{
    int numEndSpaces = 0;
    for (int i = str.length() - 1; i >= 0; i--) 
    {
        if (!isspace(str[i])) 
            break;
        
        numEndSpaces++;
    }

    return str.substr(0, str.length() - numEndSpaces);
}

// Left and right trim the given string ("  hello!  " --> "hello!")
string trim(string str) 
{
    return right_trim(left_trim(str));
}

//------------------------------------------------------------------------------------------------
// Class member functions
//------------------------------------------------------------------------------------------------

//******************************************************************************************
// @name                    : ConfigParser
//
// @description             : Constructor
//
// @returns                 : Nothing
//******************************************************************************************
ConfigParser::ConfigParser(bool debug, const string & filename)
{
    m_debug = debug;
    m_configFileName = filename;
    m_lines = 0;
    m_ignoreLines = 0;
    m_validEntries = 0;
    m_invalidEntries = 0;
}

//******************************************************************************************
// @name                    : ConfigParser
//
// @description             : Destructor
//
// @returns                 : Nothing
//******************************************************************************************
ConfigParser::~ConfigParser()
{
    m_fileStream.close();
}

//******************************************************************************************
// @name                    : ParseConfigFile
//
// @description             : This function checks for the presence of config file. It then
//                            parses it line by line. It ignores commented lines (lines starting
//                            with COMMENT_CHAR and whitespaces. It also maintains a count of
//                            total lines parsed, valid entries, invalid entries and entries to 
//                            ignore. At the end of parsing, it will display these statistics.
//                            If debug is enabled, then it show each key/value pair and a list
//                            of all the invalid entries (along with line number) present in the
//                            config file.
//
// @returns                 : Unordered map of key value pairs found in the config file.
//******************************************************************************************
unordered_map<string, string> ConfigParser::ParseConfigFile()
{
    m_fileStream.open(m_configFileName, ios::in);

    if (!m_fileStream)
    {
        printf("File [ %s ] NOT found!\n", m_configFileName.c_str());
        return m_configMap;
    }

    // Display config information
    if (isDebugEnabled())
    {
        printf("Processing [ %s ] in %s mode\n", m_configFileName.c_str(), m_debug ? "DEBUG" : "NON-DEBUG");
    }

    // Check for file size
    long configFileSize = GetFileSize();
    if (configFileSize > 0)
    {
        printf("Parsing config file [ %s ], size: %ld bytes\n\n", m_configFileName.c_str(), configFileSize);
    }
    else
    {
        printf("Config File empty\n");
        return m_configMap;
    }


    // Check if file is open
    if (!m_fileStream.is_open())
    {
        printf("Failed to parse. Config file not open\n");
        return m_configMap;
    }

    // Parse and store in unorder_map
    std::string line;
    long int lineNumber = 1;
    while (getline(m_fileStream, line))
    {
        // Trim spaces from left and right of the line
        line = trim(line);
        m_lines++;

        // Ignore the commented and whitespace lines
        if (line[0] == COMMENT_CHAR || line.length() == 0)
        {
            m_ignoreLines++;
            lineNumber++;
            continue;
        }

        string key;
        string value;
        bool validEntry = SplitLineToKeyValue(line, key, value);
        if (validEntry)
        {
            // Insert the valid entry to the config Map
            m_validEntries++;
            m_configMap[key] = value;
        }
        else
        {
            // Store invalid entries along with line number in config file
            m_invalidEntries++;
            m_invalidLines[lineNumber] = line;
        }

        lineNumber++;
    } // End of file parsing

    // Dump parsing details
    DisplayParsingDetails();
    if (isDebugEnabled())
    {
        DisplayInvalidLines();
    }

    return m_configMap;
}


//******************************************************************************************
// @name                    : SplitLineToKeyValue
//
// @description             : The function finds out if the line can be splitted to a valid
//                            key/value pair. It splits the line on the basis of the 
//                            delimiter as specified by macro KEY_VALUE_SEPARATOR. A sample 
//                            valid entry (with KEY_VALUE_SEPARATOR = '=') is: 
//                            input_file = abc.txt
//
// param @line              : Line which has to be parsed for key/value pair
// param @key               : (OUTPUT) Valid key value
// param @key               : (OUTPUT) Valid value for the key
//
// @returns                 : true if the input line was splitted into a valid key/value pair,
//                            false otherwise
//******************************************************************************************
bool ConfigParser::SplitLineToKeyValue(const string & line, string & key, string & value)
{
    // Check for delimiter
    size_t delimIndex = line.find(KEY_VALUE_SEPARATOR);
    if (delimIndex == string::npos)
    {
        // Delimiter not found, stop further parsing
        return false;
    }
    else
    {
        // Check if multiple delimiters are specified
        size_t delimIndex2 = line.find(KEY_VALUE_SEPARATOR, delimIndex + 1);
        if (delimIndex2 != string::npos)
        {
            // another delimiter found! This cannot be a valid entry
            return false;
        }
    }

    //Entry cannot start with a delimiter
    if (delimIndex == 0)
    {
        return false;
    }

    // A valid key value should have a non-zero length
    key = trim(line.substr(0, delimIndex - 1));
    value = trim(line.substr(delimIndex + 1));
    if (key.length() && value.length())
    {
        if (isDebugEnabled())
        {
            cout << "Key   : " << key << endl;
            cout << "Value : " << value << endl;
            cout << endl;
        }
        return true;
    }

    // this line could not be splitted to a valid key/value pair
    return false;
}

//******************************************************************************************
// @name                    : GetFileSize
//
// @description             : Fetches size of input configuration file.
//
// @returns                 : File size on success, -1 on error
//******************************************************************************************
long ConfigParser::GetFileSize()
{
    struct stat stat_buf;
    int retval = stat(m_configFileName.c_str(), &stat_buf);
    return retval == 0 ? stat_buf.st_size : -1;
}

//******************************************************************************************
// @name                    : DisplayParsingDetails
//
// @description             : Details corresponding to config file parsing.
//
// @returns                 : Nothing
//******************************************************************************************
void ConfigParser::DisplayParsingDetails()
{
    printf("\n");
    printf("+------------------------------------------------------------------\n");
    printf("| Config file parsing details:\n");
    printf("+------------------------------------------------------------------\n");
    printf("Config file name                   : %s\n", m_configFileName.c_str());
    printf("Lines in file                      : %ld\n", m_lines);
    printf("Valid entries                      : %ld\n", m_validEntries);
    printf("Invalid entries                    : %ld\n", m_invalidEntries);
    printf("Commented/Whitespace               : %ld\n", m_ignoreLines);
    printf("+------------------------------------------------------------------\n");
    printf("\n");
}

//******************************************************************************************
// @name                    : DisplayInvalidLines
//
// @description             : If there were any invalid lines in the config file, they 
//                            will be displayed along with line number.
//
// @returns                 : Nothing
//******************************************************************************************
void ConfigParser::DisplayInvalidLines()
{
    if (m_invalidLines.size())
    {
        printf("\n");
        printf("Invalid lines found in [ %s ]\n", m_configFileName.c_str());
        for (auto it = m_invalidLines.begin(); it != m_invalidLines.end(); it++)
        {
            printf("Line #%-3d: %s\n", it->first, (it->second).c_str());
        }
    }
}