#include "boost/filesystem.hpp"
#include <iostream>
#include <sstream>
#include <queue>

namespace fs = boost::filesystem;

class Errors
{
	public:
		std::queue<int> errorType;
		std::queue<fs::path> errorLocation;
		std::string parseError(int error)
		{
			switch(error)
			{
				case 1: return "Is missing Info.txt";
				case 2: return "Expected a period";
				case 3: return "Occurred twice";
				case 4: return "Expected a dash";
				//Error types that I would like to implement
				case 5: return "Born before death";
				case 6: return "Over 120 years old";
				case 7: return "Born before parent";
				case 8: return "Born after parent died";
				case 9: return "Marriage outside lifespan";
			}
		}
		void errorOne(std::string line, fs::path p, bool isempty)
		{
			if(isempty)
			{
				return;
			}
			if(line.find("-")==std::string::npos)
			{
				errorType.push(4);
				errorLocation.push(p);
			}
		}
		void errorTwo(fs::path info, fs::path p)
		{
			fs::ifstream file(info);
			std::string line;
			while(getline(file,line))
			{
				if(line.find('.')!=std::string::npos) return;
			}
			errorType.push(2);
			errorLocation.push(p);
		}
		void errorThree(fs::path p)
		{
			std::priority_queue<std::string> parents;
			std::string currentParent;
			//Sort and compare to see if family name occurs twice
			fs::directory_iterator end_iter;
			//How many families are there?
			//Look at everything in the top folder
			for( fs::directory_iterator first_iter(p) ; first_iter != end_iter ; ++first_iter)
			{
				//Only proceed if current item is a folder
				if(is_directory(first_iter->path()))
				{
					//Look at every folder in this folder
					for( fs::directory_iterator second_iter(first_iter->path()) ; second_iter != end_iter ; ++second_iter)
					{
						if((second_iter->path().filename().string()).find("Unmarried")==std::string::npos)
						{
							currentParent = second_iter->path().filename().string();
							while(currentParent.at(0)!='.')
							{
								currentParent.erase(0,1);
							}
							currentParent.erase(0,2);
							parents.push(currentParent);
						}
					}
				}
			}
			std::string next = parents.top();
			std::string first;
			while(!parents.empty())
			{
				first = next;
				parents.pop();
				if(parents.empty())
					break;
				next = parents.top();
				if(first==next)
				{
					//Report an error
					errorType.push(3);
					errorLocation.push(first);
				}
			}
		}
		void errorFour(fs::directory_iterator second_iter)
		{
			fs::directory_iterator end_iter;
			for( fs::directory_iterator third_iter(second_iter->path()); third_iter != end_iter; ++third_iter)
			{
				if(third_iter->path().string().find("Info.txt")!=std::string::npos)
				{
					return;
				}
			}
			errorType.push(1);
			errorLocation.push(second_iter->path());
		}
};