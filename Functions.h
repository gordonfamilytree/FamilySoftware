#include "boost/filesystem.hpp"
#include <iostream>
#include <sstream>
#include <queue>

namespace fs = boost::filesystem;

int getTotalFamilies(fs::path p)
{
	fs::directory_iterator end_iter;
	int totalFamilies = 0;
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
					totalFamilies++;
					for(fs::directory_iterator third_iter(second_iter->path());third_iter!=end_iter;++third_iter)
					{
						if(third_iter->path().filename() == "Info2.txt")
						{
							totalFamilies++;
						}
					}
				}
			}
		}
	}
	//std::cout<<"Done getting total families\n";
	return totalFamilies;
}

int getTotalPeople(fs::path p, Errors *allErrors)
{
	Person person;
	fs::directory_iterator end_iter;
	int totalPeople = 0;
	//How many families are there?
	//Look at everything in the top folder
	for( fs::directory_iterator first_iter(p) ; first_iter != end_iter ; ++first_iter)
	{
		if(first_iter->path().filename().string().find("Software")!=std::string::npos)
		{
			continue;
		}
		//Only proceed if current item is a folder
		if(is_directory(first_iter->path()))
		{
			//Look at every folder in this folder
			for( fs::directory_iterator second_iter(first_iter->path()) ; second_iter != end_iter ; ++second_iter)
			{
				(*allErrors).errorFour(second_iter);
				if((second_iter->path().filename().string()).find("Unmarried")!=std::string::npos)
				{
					for(fs::directory_iterator third_iter(second_iter->path());third_iter!=end_iter;++third_iter)
					{
						if((third_iter->path()).filename() == "Info.txt")
						{
							totalPeople = totalPeople + person.countPeople(third_iter->path());
						}
					}
				}
				else 
				{
					for(fs::directory_iterator third_iter(second_iter->path());third_iter!=end_iter;++third_iter)
					{
						if(third_iter->path().filename() == "Info2.txt")
						{
							totalPeople++;
						}
					}
					totalPeople = totalPeople + 2;
				}
			}
		}
	}
	//std::cout<<"Done getting total people\n";
	return totalPeople;
}

double getDNAEstimate(int personNumber, std::vector<Person> allPeople, std::vector<double> currentEstimates)
{
	if(personNumber == 1)
	{
		return 1;
	} 
	if( personNumber == 2 || personNumber == 3 )
	{
		return 0.5;
	}
	if( allPeople[personNumber].related == false )
	{
		return 0;
	}
	//If base, calculate based on children (use the first child listed)
	if( allPeople[personNumber].location.string().find("Base")!=std::string::npos )
	{
		//Find the index of first child
		int firstChild;
		for( int i = 1; i < allPeople.size(); i++)
		{
			if(allPeople[i].famC == allPeople[personNumber].famS)
			{
				firstChild = i;
				break;
			}
		}
		//DNA is half that of the first child
		return 0.5 * currentEstimates[firstChild];
	}
	//If not base, calculate based on parents
	//Get the DNA estimate of parents (first find the parents)
	int father;
	for(int i = 1; i < allPeople.size(); i++)
	{
		if(allPeople[i].famS == allPeople[personNumber].famC || allPeople[i].famS2 == allPeople[personNumber].famC)
		{
			father = i;
			break;
		}
	}
	int mother;
	for(int i = 1; i < allPeople.size(); i++)
	{
		if((allPeople[i].famS == allPeople[personNumber].famC || allPeople[i].famS2 == allPeople[personNumber].famC)&& i != father)
		{
			mother = i;
			break;
		}
	}
	return 0.5*getDNAEstimate(father, allPeople, currentEstimates) + 0.5*getDNAEstimate(mother, allPeople, currentEstimates);
}


