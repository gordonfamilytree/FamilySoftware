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
	return totalPeople;
}

void listPeople(std::string name, std::vector<Person> people, int totalPeople)
{
	int counter = 0;
	for(int i = 1; i < totalPeople; i++)
	{
		std::string thisName = people[i].firstName + " " + people[i].lastName;
		if(!name.compare(thisName))
		{
			counter++;
			std::cout<<counter<<". "<<people[i].location.string()<<"\n";			
		}
	}
}

int whatNumber(std::string name, std::vector<Person> people, int totalPeople, int whatPerson)
{
	int counter = 0;
	for(int i = 1; i < totalPeople; i++)
	{
		std::string thisName = people[i].firstName + " " + people[i].lastName;
		if(!name.compare(thisName))
		{
			counter++;
			if(counter == whatPerson)
			{
				return people[i].personNumber;
			}
		}
	}
}

void segmentPeople(int currentPerson, std::vector<Person> people, std::vector<int> *alreadySegmented, std::vector<Person> *segmentedPeople, bool direct, int lastDir, int depth)
{
	
	for( int i = 0; i < (*alreadySegmented).size(); i++)
	{
		if((*alreadySegmented)[i] == currentPerson)
		{
			return;
		}
	}	
	people[currentPerson].currentDir = lastDir;
	people[currentPerson].depth = depth;
	(*segmentedPeople).push_back(people[currentPerson]);
	(*alreadySegmented).push_back(currentPerson);
	//Look for children, siblings, and parents
	//Don't look at parents unless direct ancestor
	int originalLastDir = lastDir;
	for(int i = 1; i < people.size(); i++)
	{
		//Parents
		if((people[i].famS == people[currentPerson].famC || people[i].famS2 == people[currentPerson].famC) && people[currentPerson].famC > 0)
		{
			lastDir = originalLastDir;
			if(direct || (*alreadySegmented).front() == currentPerson )
			{
				segmentPeople(i,people,alreadySegmented,segmentedPeople, true, 1,depth+1);
			}				
		}
		//Siblings
		else if(people[i].famC == people[currentPerson].famC && people[currentPerson].famC > 0)
		{
			//make base siblings not direct
			if(direct)
				lastDir = originalLastDir + 1;
			else
				lastDir = originalLastDir;
			segmentPeople(i,people,alreadySegmented,segmentedPeople, false, lastDir, depth);
		}
		//Children
		else if( (people[i].famC == people[currentPerson].famS || people[i].famC == people[currentPerson].famS2) && people[i].famC > 0)
		{
			lastDir = originalLastDir + 1;
			segmentPeople(i,people,alreadySegmented,segmentedPeople, false, lastDir, depth-1);
		}
		//Spouse
		else if( (people[i].famS == people[currentPerson].famS || people[i].famS == people[currentPerson].famS2) && people[i].famS > 0)
		{
			if(!direct)
			{
				//Add this person too but don't look at their relatives
				(*segmentedPeople).push_back(people[i]);
				(*alreadySegmented).push_back(i);
				people[currentPerson].currentDir = originalLastDir;
			}
		}
	}
}

void performChanges(int currentPerson, int currentFamily, std::vector<Person> *segmentedPeople, std::string saveLocation)
{
	//Return at the last person
	if(currentPerson == (*segmentedPeople).size())
		return;	
	//Figure out what subfolder they should go in also
	fs::path currentPath;
	if((*segmentedPeople)[currentPerson].currentDir == 1)
	{		
		std::string end = (*segmentedPeople)[currentPerson].location.filename().string();
		end[0] = abs((*segmentedPeople)[currentPerson].depth)+48;
		if((*segmentedPeople)[currentPerson].depth<0)
		{
			end = '-'+end;
		}
		currentPath = fs::path(saveLocation+"/1. Base/"+end);
		fs::create_directory(currentPath);	
	}
	else if((*segmentedPeople)[currentPerson].currentDir == 2)
	{
		std::string end = (*segmentedPeople)[currentPerson].location.filename().string();
		end[0] = (char)(*segmentedPeople)[currentPerson].depth+48;
		currentPath = fs::path(saveLocation+"/2. Aunts and Uncles/"+end);
		fs::create_directory(currentPath);			
	}
	else if((*segmentedPeople)[currentPerson].currentDir == 3)
	{
		std::string end = (*segmentedPeople)[currentPerson].location.filename().string();
		end[0] = (char)(*segmentedPeople)[currentPerson].depth+48;
		currentPath = fs::path(saveLocation+"/3. First Cousins/"+end);
		fs::create_directory(currentPath);		
	}
	else if((*segmentedPeople)[currentPerson].currentDir == 4)
	{
		std::string end = (*segmentedPeople)[currentPerson].location.filename().string();
		end[0] = (char)(*segmentedPeople)[currentPerson].depth+48;
		currentPath = fs::path(saveLocation+"/4. Second Cousins/"+end);
		fs::create_directory(currentPath);		
	}
	else if((*segmentedPeople)[currentPerson].currentDir == 5)
	{
		std::string end = (*segmentedPeople)[currentPerson].location.filename().string();
		end[0] = (char)(*segmentedPeople)[currentPerson].depth+48;
		currentPath = fs::path(saveLocation+"/5. Third Cousins/"+end);
		fs::create_directory(currentPath);		
	}	
	//Copy all files from the old directory to the new directory
	fs::directory_iterator end_iter;
	for( fs::directory_iterator first_iter((*segmentedPeople)[currentPerson].location) ; first_iter != end_iter ; ++first_iter)
	{
		fs::path oldPath = first_iter->path();
		fs::path newPath(currentPath.string()+"/"+first_iter->path().filename().string());
		//Spouse or siblings ruins this because the file already exists
		if(!fs::exists(newPath))
		{
			fs::copy_file(oldPath,newPath);
		}
	}
	//Change the location of this person to the new location
	(*segmentedPeople)[currentPerson].location = currentPath;
	//Change all of the references
	std::queue<std::string> nq;
	for(int i = 0; i < (*segmentedPeople)[currentPerson].resourcesFile.size(); i++)
	{
		std::string old = (*segmentedPeople)[currentPerson].resourcesFile.front();
		(*segmentedPeople)[currentPerson].resourcesFile.pop();
		fs::path r(old);
		fs::path nr(currentPath.string() + r.filename().string());
		nq.push(nr.string());
	}
	(*segmentedPeople)[currentPerson].resourcesFile = nq;
	//Change the person number of this person
	(*segmentedPeople)[currentPerson].personNumber = currentPerson+1;
	//Change the family numbers of this person (store prior)
	if(!(*segmentedPeople)[currentPerson].changedS)
	{
		if((*segmentedPeople)[currentPerson].famS > 0)
		{
			int oldFamS = (*segmentedPeople)[currentPerson].famS;
			(*segmentedPeople)[currentPerson].famS = currentFamily;
			(*segmentedPeople)[currentPerson].changedS = true;
			//Spouse
			for(int i = 0; i < (*segmentedPeople).size(); i++)
			{
				if(((*segmentedPeople)[i].famS == oldFamS) && !(*segmentedPeople)[i].changedS)
				{
					(*segmentedPeople)[i].famS = currentFamily;
					(*segmentedPeople)[i].changedS = true;
					break;
				}
				else if(((*segmentedPeople)[i].famS2 == oldFamS) && !(*segmentedPeople)[i].changedS)
				{
					(*segmentedPeople)[i].famS2 = currentFamily;
					(*segmentedPeople)[i].changedS = true;
					break;
				}
			}
			//Children
			for(int i = 0; i < (*segmentedPeople).size(); i++)
			{
				if((*segmentedPeople)[i].famC == oldFamS && !(*segmentedPeople)[i].changedC)
				{
					(*segmentedPeople)[i].famC = currentFamily;
					(*segmentedPeople)[i].changedC = true;
				}
			}		
			currentFamily++;
		}
		if((*segmentedPeople)[currentPerson].famS2 > 0)
		{
			int oldFamS2 = (*segmentedPeople)[currentPerson].famS2;
			(*segmentedPeople)[currentPerson].famS = currentFamily;
			//Spouse
			for(int i = 0; i < (*segmentedPeople).size(); i++)
			{
				if((*segmentedPeople)[i].famS == oldFamS2 && !(*segmentedPeople)[i].changedS)
				{
					(*segmentedPeople)[i].famS = currentFamily;
					(*segmentedPeople)[i].changedS = true;
					break;
				}
			}
			//Children
			for(int i = 0; i < (*segmentedPeople).size(); i++)
			{
				if((*segmentedPeople)[i].famC == oldFamS2 && !(*segmentedPeople)[i].changedC)
				{
					(*segmentedPeople)[i].famC = currentFamily;
					(*segmentedPeople)[i].changedC = true;
				}
			}	
			currentFamily++;
		}
	}
	currentPerson++;
	performChanges(currentPerson, currentFamily, segmentedPeople, saveLocation);
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
		std::vector<Person> segmentedPeople;
		std::vector<Person>*passPeople = &segmentedPeople;
		std::vector<Family> segmentedFamilies;
		std::vector<Family>*passFamilies = &segmentedFamilies;
		std::vector<int> alreadySegmented;
		std::vector<int>*passAlready = &alreadySegmented;
		std::vector<double> DNAestimates(totalPeople);
		int percentDone;
		std::cout<<"Getting Ready\n";
	//Gather all information
		for(int i = 1; i < totalPeople; i++)
		{
			//Display percent done
			percentDone = (100*i/(totalPeople));
			std::cout<<percentDone<<"%\r";
			people[i].gatherInfo(i,passErrors,p);		
		}
		std::cout<<"100%\n";
	//Get the person to segment around
		//Get person's name
			std::cout << "Enter the name to segment around\n";
			std::string segmentPerson;
			std::getline(std::cin , segmentPerson);
		//Output list of people with that name
			listPeople(segmentPerson, people, totalPeople);
		//Get the appropriate one
			std::cout << "Which person did you mean?";
			int whichPerson;
			std::cin >> whichPerson;
		//Get number of person to segment about
			int segmentNumber = whatNumber(segmentPerson,people,totalPeople,whichPerson);
	//Segment around that person
		//Where do they get saved?
		std::cout<<"Finding Relatives\n";
		std::string saveLocation = fs::current_path().string() + "/" + segmentPerson;
		segmentPeople(segmentNumber, people, passAlready, passPeople, false, 1, 0);
		//Change the person numbers, family numbers, family paths (creating folders while doing so)
		fs::path currentPath(saveLocation);
		fs::create_directory(currentPath);
		fs::path base(saveLocation+"/1. Base");
		fs::create_directory(base);
		fs::path au(saveLocation+"/2. Aunts and Uncles");
		fs::create_directory(au);
		fs::path c1(saveLocation+"/3. First Cousins");
		fs::create_directory(c1);
		fs::path c2(saveLocation+"/4. Second Cousins");
		fs::create_directory(c2);
		fs::path c3(saveLocation+"/5. Third Cousins");
		fs::create_directory(c3);
		std::cout<<"Correcting information\n";
		performChanges(0,1,passPeople,saveLocation);
		//Store the segmentedFamilies information
		for(int i = 0; i < segmentedPeople.size(); i++)
		{
			if(!segmentedPeople[i].changedC)
				segmentedPeople[i].famC = 0;
		}
		Person tempPerson;
		segmentedPeople.insert(segmentedPeople.begin(),tempPerson);
		std::cout<<"Updating relationships\n";
		//Also get updated relationships and DNA estimate
		for(int i = 1; i < segmentedPeople.size(); i++)
		{
			//Get relationship next
			fs::directory_iterator what_iter;
			fs::directory_iterator end_iter;
			for(fs::directory_iterator first_iter(p); first_iter!=end_iter; ++first_iter)
			{
				if(saveLocation.find(first_iter->path().string()))
				{
					what_iter = first_iter;
					break;
				}
			}
			fs::directory_iterator what_iter3;
			for(fs::directory_iterator third_iter(segmentedPeople[i].location); third_iter != end_iter; ++third_iter)
			{
				if(segmentedPeople[i].info2)
				{
					if(third_iter->path().string().find("Info2.txt"))
					{
						what_iter3 = third_iter;
						break;
					}
				}
				else
				{
					if(third_iter->path().string().find("Info.txt"))
					{
						what_iter3 = third_iter;
						break;
					}
				}
			}
			segmentedPeople[i].getRelationship(what_iter, segmentedPeople[i].location.filename().string(), what_iter3 );
			Family temp;		
			if(temp.exists(i,segmentedPeople))
			{
				temp.gatherInfo(segmentedPeople,i);
				segmentedFamilies.push_back(temp);
			}
		}	
		for(int i = 1; i < segmentedPeople.size(); i++)
		{
			DNAestimates[i] = getDNAEstimate(i, segmentedPeople, DNAestimates);		
		}		
	//Open all output files		
		//Error log
		fs::path errorOutput(saveLocation+"/ErrorLog.txt");
		fs::ofstream errorsFile;
		errorsFile.open(errorOutput);
		errorsFile<<"List of Errors:\n\n";
		//Index
		fs::path indexOutput(saveLocation+"/Index.csv");
		fs::ofstream indexFile;
		indexFile.open(indexOutput);
		indexFile<<"Number,First Name,Middle Name,Last Name,Birth Year, Relation, DNA Estimate\n";
		//Tree
		fs::path treeOutput(saveLocation+"/Tree.ged");
		fs::ofstream treeFile;
		treeFile.open(treeOutput);
		treeFile<<"0 HEAD\n1 CHAR ASCII\n1 LANG English\n1 SOUR 0\n1 SUBM @U@\n1 GEDC\n2 VERS 5.5\n2 FORM LINEAGE-LINKED\n0 @U@ SUBM\n1 NAME Robert Gordon\n";
	//Output all information
		//Output person information
			for(int i = 0; i < segmentedPeople.size(); i++)
			{
				if(segmentedPeople[i].firstName == "" && segmentedPeople[i].middleName == "" && segmentedPeople[i].lastName == "")
				{
					//Person does not exist
					continue;
				}
				//To gedcom
				treeFile<<"0 @P"<<segmentedPeople[i].personNumber<<"@ INDI\n";
				treeFile<<"1 BIRT\n2 DATE "<<segmentedPeople[i].birthMonth<<" "<<segmentedPeople[i].birthDay<<" "<<segmentedPeople[i].birthYear<<"\n2 PLAC "<<segmentedPeople[i].birthPlace<<"\n";
				if(segmentedPeople[i].deathYear != "")
				{
					treeFile<<"1 DEAT\n2 DATE "<<segmentedPeople[i].deathMonth<<" "<<segmentedPeople[i].deathDay<<" "<<segmentedPeople[i].deathYear<<"\n2 PLAC "<<segmentedPeople[i].deathPlace<<"\n";
				}
				treeFile<<"1 SEX "<<segmentedPeople[i].sex<<"\n";
				treeFile<<"1 NAME "<<segmentedPeople[i].firstName<<" "<<segmentedPeople[i].middleName<<" /"<<segmentedPeople[i].lastName<<"/\n";
				if(segmentedPeople[i].marriage != "")
				{
					treeFile<<"1 MARR\n2 DATE "<<segmentedPeople[i].marriage<<"\n";
				}
				if(segmentedPeople[i].famS != 0)
				{
					treeFile<<"1 FAMS @F"<<segmentedPeople[i].famS<<"@\n";
				}
				if(segmentedPeople[i].famC != 0)
				{
					treeFile<<"1 FAMC @F"<<segmentedPeople[i].famC<<"@\n";
				}
				for(int j = 0; j < (segmentedPeople[i].resourcesTitle).size(); j++)
				{
					//Add the resource
					treeFile<<"1 OBJE\n2 FILE "<<(segmentedPeople[i].resourcesFile).front()<<"\n2 FORM "<<(segmentedPeople[i].resourcesForm).front()<<"\n2 TITL "<<(segmentedPeople[i].resourcesTitle).front()<<"\n";
					(segmentedPeople[i].resourcesFile).pop();
					(segmentedPeople[i].resourcesForm).pop();
					(segmentedPeople[i].resourcesTitle).pop();
				}
				//To index
				indexFile<<segmentedPeople[i].personNumber<<","<<segmentedPeople[i].firstName<<","<<segmentedPeople[i].middleName<<","<<segmentedPeople[i].lastName<<","<<segmentedPeople[i].birthYear<<","<<people[i].relationship<<","<<DNAestimates[i]<<"\n";
			}
		//Output family information
			for(int i = 0; i < segmentedFamilies.size(); i++)
			{
				//Parents
				treeFile<<"0 @F"<<segmentedFamilies[i].familyNumber<<"@ FAM\n1 HUSB @P"<<segmentedFamilies[i].husband<<"@\n1 WIFE @P"<<segmentedFamilies[i].wife<<"@\n"; 
				//Children
				while(!(segmentedFamilies[i].children.empty()))
				{
					treeFile<<"1 CHIL @P"<<segmentedFamilies[i].children.front()<<"@\n2 _FREL ";
					segmentedFamilies[i].children.pop();
					if(segmentedFamilies[i].natural.front())
					{
						treeFile<<"Natural\n2 _MREL Natural\n";
					}
					else
					{
						treeFile<<"Adopted\n2 _MREL Adopted\n";
					}
					segmentedFamilies[i].natural.pop();
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
