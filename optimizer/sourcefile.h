#include <string>
#include <vector>

using std::string;
using std::vector;

class SourceFile
{
  public:
    SourceFile() 
    {
        m_index = 0;
    }

    void Insert(string line)
    {
        LineItem *item = new LineItem(line);
        m_file.push_back(item);
    }

    LineItem *next(size_t pos)
    {
        m_index = pos;
        return next();
    }

    LineItem *next()
    {
        return m_file[m_index++];
    }

    bool EndOfFile() { return m_index >= m_file.size(); }

    size_t current() { return m_index; }
  protected:
    vector<LineItem *> m_file;
    size_t m_index;
};
