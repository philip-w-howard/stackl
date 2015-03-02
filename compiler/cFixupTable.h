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
        std::pair<std::string,int> mypair (label, loc);
        mSource.insert(mypair);
    }
    void AddJumpDest(std::string label, int loc)
    {
        mDest[label] = loc;
    }

    void FixupOutput(std::ofstream &output)
    {
        std::string name;

        std::unordered_multimap<std::string, int>::iterator it;
        std::unordered_map<std::string, int>::const_iterator found;
        for (it=mSource.begin(); it!=mSource.end(); it++)
        {
            name = (*it).first;

            found = mDest.find(name);
            if (found != mDest.end())
            {
                output << "F " << (*it).second << " " 
                    << (*found).second << "\n";
            } else {
                std::cerr << "Internal compiler error: no destination for "
                    << name << " in fixup table" << std::endl;
            }
        }
    }

  protected:
    std::unordered_multimap<std::string, int> mSource;
    std::unordered_map<std::string, int> mDest;
};
