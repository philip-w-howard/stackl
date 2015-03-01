#include <fstream>
#include <unordered_map>
#include <string>

class cFixupTable
{
  public:
    cFixupTable()
    {}

    void AddJumpSource(std::string label, int loc)
    {
        mSource[label] = loc;
    }
    void AddJumpDest(std::string label, int loc)
    {
        mDest[label] = loc;
    }

    void FixupOutput(std::ofstream &output)
    {
        std::string name;

        std::unordered_map<std::string, int>::const_iterator found;
        std::unordered_map<std::string, int>::iterator it;
        for (it=mSource.begin(); it!=mSource.end(); it++)
        {
            name = (*it).first;

            found = mDest.find(name);
            if (found == mDest.end())
            {
                std::cerr << "Internal compiler error: no destination for " 
                          << name << " in fixup table" << std::endl;
            } else {
                output << "F " << (*it).second << " " << found->second << "\n";
            }
        }
    }

  protected:
    std::unordered_map<std::string, int> mSource;
    std::unordered_map<std::string, int> mDest;
};
