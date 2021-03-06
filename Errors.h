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

		int marriageYoung = 15;
		
		bool notException(fs::path exceptions)
		{
			std::string line;
			fs::ifstream exceptionsFile(exceptions);
			while(getline(exceptionsFile, line))
			{
				std::string str = "\"" + errorLocation.front().string() + "\"";
				if(line == str)
					return false;
			}
			return true;
		}
		std::string parseError(int error)
		{
			switch(error)
			{
				case 1: return "Is missing Info.txt";
				case 2: return "Expected a period";
				case 3: return "Occurred twice";
				case 4: return "Expected a dash";				
				case 5: return "Born before death";
				case 6: return "Over 120 years old";
				case 7: return "Born before parent";
				case 8: return "Born after parent died";
				case 9: return "Marriage outside lifespan";				
				case 10: return "No connected families";
				case 11: return "Children surnames don't match";
				case 12: return "Born before parents were married";
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
		void errorFiveSix(int birth, int death, fs::path p)
		{
			if(death == 0 || birth == 0)
				return;
			if(death < birth)
			{
				errorType.push(5);
				errorLocation.push(p);
			}
			else if(death > birth + 119)
			{
				errorType.push(6);
				errorLocation.push(p);
			}
		}
		void errorSevenEight(std::queue<int> childrenBirths, std::queue<fs::path> childrenPaths, int fatherBirth, int motherBirth, int fatherDeath, int motherDeath)
		{
			while( childrenBirths.size() > 0 )
			{
				int birth = childrenBirths.front();
				fs::path p = childrenPaths.front();

				childrenBirths.pop();
				childrenPaths.pop();

				if(birth == 0)
					continue;
				if(birth < fatherBirth && fatherBirth > 0 || birth < motherBirth && motherBirth > 0)
				{
					errorType.push(7);
					errorLocation.push(p);
				}
				if(birth > fatherDeath && fatherDeath> 0 || birth > motherDeath && motherDeath > 0)
				{
					errorType.push(8);
					errorLocation.push(p);
				} 
			}
		}
		void errorNine(int marriage, int birth, int death, fs::path p)
		{
			if(marriage == 0)
				return;
			if(birth == 0 && death == 0)
				return;

			if(marriage - marriageYoung < birth && birth > 0)
			{
				errorType.push(9);
				errorLocation.push(p);
			}

			if(marriage > death && death > 0)
			{
				errorType.push(9);
				errorLocation.push(p);
			}
		}
		void errorTen(int famC, int famS, int famS2, fs::path p)
		{
			if(famC + famS + famS2 == 0)
			{
				errorType.push(10);
				errorLocation.push(p);
			}
		}
		void errorEleven(std::queue<std::string> childrenSurnames, std::string fatherSurname, std::string motherSurname, std::queue<fs::path> childrenPaths)
		{
			while(childrenSurnames.size() > 0)
			{
				std::string surname = childrenSurnames.front();
				fs::path p = childrenPaths.front();

				childrenSurnames.pop();
				childrenPaths.pop();

				if(surname != fatherSurname && surname != "" && fatherSurname != "")
				{
					if(surname != motherSurname && surname != "" && motherSurname != "")
					{
						errorType.push(11);
						errorLocation.push(p);
					}
				}
			}
		}
		void errorTwelve(std::queue<int> childrenBirths, int marriage, std::queue<fs::path> childrenPaths)
		{
			while( childrenBirths.size() > 0 )
			{
				int birth = childrenBirths.front();
				fs::path p = childrenPaths.front();

				childrenBirths.pop();
				childrenPaths.pop();

				if(birth == 0)
					continue;
				if(birth < marriage && marriage > 0)
				{
					errorType.push(12);
					errorLocation.push(p);
				}
			}
		}
};