#include "boost/filesystem.hpp"
#include <iostream>
#include <sstream>
#include <queue>

namespace fs = boost::filesystem;

class Family
{
	public:
		//---------------------------------Variables------------------------------
		std::string husbandName = "";
		//Family Number
		int familyNumber = 0;
		//Parents
		int husband = 0;
		int wife = 0;
		//Marriage Date
		std::string marriage = "";
		//Children
		std::queue<int> children;
		std::queue<int> childrenBirths;
		std::queue<fs::path> childrenPaths;
		std::queue<std::string> childrenSurnames;
		std::queue<bool> natural;
		//------------------------------------------------------------------------
		//---------------------------------Functions------------------------------		
		void gatherInfo(std::vector<Person> people, int familyCounter, Errors *allErrors)
		{
			familyNumber = familyCounter;
			getParents(people,familyCounter);
			getChildren(people,familyCounter);
			(*allErrors).errorSevenEight(childrenBirths, childrenPaths, stringtoint(people[husband].birthYear),
				stringtoint(people[wife].birthYear),stringtoint(people[husband].deathYear),stringtoint(people[wife].deathYear));
			(*allErrors).errorEleven(childrenSurnames,people[husband].lastName,people[wife].lastName,childrenPaths);
			(*allErrors).errorTwelve(childrenBirths,stringtoint(marriage),childrenPaths);
		}
		bool exists(int familyCounter, std::vector<Person> people)
		{
			for(int i = 0; i < people.size(); i++)
			{
				if(people[i].famS == familyCounter || people[i].famS2 == familyCounter)
					return true;
			}
			return false;
		}
		//------------------------------------------------------------------------
	private:
		//---------------------------------Functions------------------------------		
		void getParents(std::vector<Person> people, int familyCounter)
		{
			for(int i = 0; i < people.size(); i++)
			{
				if(people[i].famS == familyCounter)
				{
					if(people[i].sex == "M" || people[i].firstName == "Sherry" && people[i].lastName == "Taylor")
					{
						husband = i;
						husbandName = people[i].firstName+" "+people[i].lastName;
					}
					else{
						wife = i;
					}
					marriage = people[i].marriage;
				}
				else if(people[i].famS2 == familyCounter)
				{
					if(people[i].sex == "M")
					{
						husband = i;
						husbandName = people[i].firstName+" "+people[i].lastName;
					}
					else
					{
						wife = i;
						husbandName = people[i+1].firstName+" "+people[i+1].lastName;
					}
					marriage = people[i].marriage;
				}
				if(husband > 0 && wife > 0)
				{
					return;
				}
			}
		}
		void getChildren(std::vector<Person> people, int familyCounter)
		{
			for(int i = 1; i < people.size(); i++)
			{
				if(people[i].famC == familyCounter)
				{
					children.push(i);
					childrenBirths.push(stringtoint(people[i].birthYear));
					childrenPaths.push(people[i].location);
					childrenSurnames.push(people[i].lastName);
					if( husbandName == "William Parks" )//Change this if any other adopted people appear
					{
						natural.push(false);
					}
					else
					{
						natural.push(true);
					}
				}
			}
		}
		//------------------------------------------------------------------------
};