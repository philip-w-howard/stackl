#include <string>
#include <string.h>
using std::string;

static const char *DELIMS = " \t\n\r";

class LineItem
{
  public:
    LineItem(string data) : line(data)
    {}

    string text() { return line; }

    string first()
    {
        char data[256];
        strcpy(data, line.c_str());
        char *token = strtok(data, DELIMS);
        if (token == NULL)
            return string("");
        else
            return string(token);
    }

    string second()
    {
        char data[256];
        strcpy(data, line.c_str());
        char *token = strtok(data, DELIMS);
        token = strtok(NULL, DELIMS);
        if (token == NULL)
            return string("");
        else
            return string(token);
    }

  protected:
    string line;
};
