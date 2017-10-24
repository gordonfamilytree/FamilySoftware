#include "boost/filesystem.hpp"
#include <iostream>
#include <sstream>
#include <queue>
#include "Errors.h"
#include "Person.h"
#include "Family.h"

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

int main()
{
	//Variables
		int personCounter = 0;
		int familyCounter = 0;
		Errors allErrors;
		Errors *passErrors = &allErrors;
		fs::path p(fs::current_path());
		int totalPeople = getTotalPeople(p, passErrors);
		int totalFamilies = getTotalFamilies(p);
		std::vector<Person> people(totalPeople);
		std::vector<Family> families(totalFamilies);
		std::vector<double> DNAestimates(totalPeople);
		int percentDone;
	//Open all output files
		//Error log
		fs::path errorOutput(fs::current_path().string()+"/ErrorLog.txt");
		fs::ofstream errorsFile;
		errorsFile.open(errorOutput);
		errorsFile<<"List of Errors:\n\n";
		//Index
		fs::path indexOutput(fs::current_path().string()+"/Index.csv");
		fs::ofstream indexFile;
		indexFile.open(indexOutput);
		indexFile<<"Number,First Name,Middle Name,Last Name,Birth Year, Relation, DNA Estimate\n";
		//Tree
		fs::path treeOutput(fs::current_path().string()+"/Tree.ged");
		fs::ofstream treeFile;
		treeFile.open(treeOutput);
		treeFile<<"0 HEAD\n1 CHAR ASCII\n1 LANG English\n1 SOUR 0\n1 SUBM @U@\n1 GEDC\n2 VERS 5.5\n2 FORM LINEAGE-LINKED\n0 @U@ SUBM\n1 NAME Robert Gordon\n";
	//Gather all information
		for(int i = 1; i < totalPeople; i++)
		{
			//Display percent done
			percentDone = (100*i/(totalPeople+totalPeople+totalFamilies));
			std::cout<<percentDone<<"%\r";
			people[i].gatherInfo(i,passErrors,p);		
		}
		for(int i = 1; i < totalFamilies; i++)
		{
			//Display percent done
			percentDone = (100*(i+totalPeople)/(totalPeople+totalPeople+totalFamilies));
			std::cout<<percentDone<<"%\r";
			families[i].gatherInfo(people, i);
		}
		for(int i = 1; i < totalPeople; i++)
		{
			DNAestimates[i] = getDNAEstimate(i, people, DNAestimates);			
			percentDone = (100*(i+totalPeople+totalFamilies)/(totalPeople+totalPeople+totalFamilies));
			std::cout<<percentDone<<"%\r";
		}
	//Output all information
		//Output person information
			for(int i = 1; i < totalPeople; i++)
			{
				if(people[i].firstName == "" && people[i].middleName == "" && people[i].lastName == "")
				{
					//Person does not exist
					continue;
				}
				//To gedcom
				treeFile<<"0 @P"<<people[i].personNumber<<"@ INDI\n";
				treeFile<<"1 BIRT\n2 DATE "<<people[i].birthMonth<<" "<<people[i].birthDay<<" "<<people[i].birthYear<<"\n2 PLAC "<<people[i].birthPlace<<"\n";
				if(people[i].deathYear != "")
				{
					treeFile<<"1 DEAT\n2 DATE "<<people[i].deathMonth<<" "<<people[i].deathDay<<" "<<people[i].deathYear<<"\n2 PLAC "<<people[i].deathPlace<<"\n";
				}
				treeFile<<"1 SEX "<<people[i].sex<<"\n";
				treeFile<<"1 NAME "<<people[i].firstName<<" "<<people[i].middleName<<" /"<<people[i].lastName<<"/\n";
				if(people[i].marriage != "")
				{
					treeFile<<"1 MARR\n2 DATE "<<people[i].marriage<<"\n";
				}
				if(people[i].famS != 0)
				{
					treeFile<<"1 FAMS @F"<<people[i].famS<<"@\n";
				}
				if(people[i].famS2 != 0)
				{
					treeFile<<"1 FAMS @F"<<people[i].famS2<<"@\n";
				}
				if(people[i].famC != 0)
				{
					treeFile<<"1 FAMC @F"<<people[i].famC<<"@\n";
				}
				for(int j = 0; j < (people[i].resourcesTitle).size(); j++)
				{
					//Add the resource
					treeFile<<"1 OBJE\n2 FILE "<<(people[i].resourcesFile).front()<<"\n2 FORM "<<(people[i].resourcesForm).front()<<"\n2 TITL "<<(people[i].resourcesTitle).front()<<"\n";
					(people[i].resourcesFile).pop();
					(people[i].resourcesForm).pop();
					(people[i].resourcesTitle).pop();
				}
				//To index
				indexFile<<people[i].personNumber<<","<<people[i].firstName<<","<<people[i].middleName<<","<<people[i].lastName<<","<<people[i].birthYear<<","<<people[i].relationship<<","<<DNAestimates[i]<<"\n";
			}
		//Output family information
			for(int i = 1; i < totalFamilies; i++)
			{
				//Parents
				treeFile<<"0 @F"<<families[i].familyNumber<<"@ FAM\n1 HUSB @P"<<families[i].husband<<"@\n1 WIFE @P"<<families[i].wife<<"@\n"; 
				//Children
				while(!(families[i].children.empty()))
				{
					treeFile<<"1 CHIL @P"<<families[i].children.front()<<"@\n2 _FREL ";
					families[i].children.pop();
					if(families[i].natural.front())
					{
						treeFile<<"Natural\n2 _MREL Natural\n";
					}
					else
					{
						treeFile<<"Adopted\n2 _MREL Adopted\n";
					}
					families[i].natural.pop();
				}
			}
		//Output error information
			while(!allErrors.errorType.empty())
			{
				errorsFile<<allErrors.errorLocation.front()<<"\n------------------"<<allErrors.parseError(allErrors.errorType.front())<<"\n";	
				allErrors.errorType.pop();
				allErrors.errorLocation.pop();
			}
	//Close all output files
		treeFile<<"0 TRLR";
		errorsFile.close();
		treeFile.close();
		indexFile.close();
		std::string open = "notepad.exe "+p.string()+"/ErrorLog.txt";
		system(open.c_str());
}