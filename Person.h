#include "boost/filesystem.hpp"
#include <iostream>
#include <sstream>
#include <queue>

namespace fs = boost::filesystem;

int stringtoint(std::string s)
{
	if(s.find(",") != std::string::npos)
	{
		s = s.substr(s.size()-4,s.size());
	}

	int result = 0;
	int multiplier = 0;
	for( int i = 0; i < 4; i++ )
	{
		if(s.length() == 0 )
			break;
		char backvalue = s.back();
		switch(backvalue)
		{
			case '1' :
				result = result + 1*pow(10,multiplier);
				break;
			case '2' :
				result = result + 2*pow(10,multiplier);
				break;
			case '3' :
				result = result + 3*pow(10,multiplier);
				break;
			case '4' :
				result = result + 4*pow(10,multiplier);
				break;
			case '5' :
				result = result + 5*pow(10,multiplier);
				break;
			case '6' :
				result = result + 6*pow(10,multiplier);
				break;
			case '7' :
				result = result + 7*pow(10,multiplier);
				break;
			case '8' :
				result = result + 8*pow(10,multiplier);
				break;
			case '9' :
				result = result + 9*pow(10,multiplier);
				break;
			default :
				result = result + 0;
				break;
		}
		multiplier++;
		s.pop_back();
	}
	return result;
}

class Person
{
	public:
		//---------------------------------Variables------------------------------
		//Name
		bool changedS = false;
		bool changedC = false;
		bool info2 = false;
		std::string firstName = "";
		std::string middleName = "";
		std::string lastName = "";
		std::string sex = "";
		//Birth
		std::string birthDay = "";
		std::string birthMonth = "";
		std::string birthYear = "";
		std::string birthPlace = "";
		//Death
		std::string deathDay = "";
		std::string deathMonth = "";
		std::string deathYear = "";
		std::string deathPlace = "";
		//Marriage
		std::string marriage = "";
		std::string relationship = "";
		bool related = true;
		bool half = false;
		//Families
		int famC = 0;
		int famS = 0;
		int famS2 = 0;
		int personNumber = 0;
		//Resources
		std::queue<std::string> resourcesFile;
		std::queue<std::string> resourcesForm;
		std::queue<std::string> resourcesTitle;
		//Location of the folder that person is in
		fs::path location;
		//Used when segmenting
		int currentDir = 0;
		int depth = -1;
		//------------------------------------------------------------------------
		//---------------------------------Functions------------------------------
		void gatherInfo(int personCounter, Errors *allErrors, fs::path p)
		{
			gatherInfoCall(personCounter, allErrors, p);
			(*allErrors).errorTen(famC, famS, famS2, location);
		}
		void gatherInfoCall(int personCounter, Errors *allErrors, fs::path p)
		{
			bool unmarried = false;
			personNumber = personCounter;
			//Go through and find the folder that the person is in
			fs::directory_iterator end_iter;
			int personCounter2 = 0;
			int familyCounter = 0;
			std::string parents;
			//Look at everything in the top folder
			for( fs::directory_iterator first_iter(p) ; first_iter != end_iter ; ++first_iter)
			{
				//Only proceed if current item is a folder
				if(is_directory(first_iter->path()))
				{
					//Look at every folder in this folder
					for( fs::directory_iterator second_iter(first_iter->path()) ; second_iter != end_iter ; ++second_iter)
					{
						//Look at every file in this folder
						for( fs::directory_iterator third_iter(second_iter->path()); third_iter != end_iter; ++third_iter)
						{							
							unmarried = false;
							//Gather information from Info.txt and output it to Index.csv
							if((third_iter->path()).filename() == "Info.txt")
							{								
								fs::path info = third_iter->path();
								fs::ifstream file(info);
								if(second_iter->path().filename().string().find("Unmarried")!=std::string::npos)
								{
									personCounter2 = personCounter2+countPeople(info);
									unmarried = true;
								}
								else 
								{
									personCounter2 = personCounter2+2;
									familyCounter++;
								}
								if(personCounter2 >= personCounter)
								{									
									//This is where the person is
									location = second_iter->path();
									(*allErrors).errorFour(second_iter);
									std::string line;
									int personCounter3 = 0;
									int lineCounter = 0;
									while(getline(file,line))
									{
										if(line.empty()) continue;
										lineCounter++;
										if(isalpha(line.at(0))||line.at(0)=='?')
										{
											personCounter3++;										
											if(unmarried && countPeople(info)-personCounter3 == personCounter2 - personCounter)
											{
												//This is the specific person
												getName(line);
												getSex(line,lineCounter,unmarried);
												related = true;
												getline(file,line);
												getBirth(line);
												(*allErrors).errorOne(line,second_iter->path(),((birthDay+birthMonth+birthYear+deathPlace)==""));
												getline(file,line);
												getDeath(line);
												(*allErrors).errorOne(line,second_iter->path(),((deathDay+deathMonth+deathYear+deathPlace)==""));
												(*allErrors).errorFiveSix(stringtoint(birthYear),stringtoint(deathYear), second_iter->path());
												getline(file,line);
												parents = getParents(line);	
												famC = getFamily(parents, p);
												addResources(second_iter->path());
												getRelationship(first_iter, second_iter->path().filename().string(), third_iter);
												return;
											}
											else if(!unmarried && 2 - personCounter3 == personCounter2 - personCounter)
											{
												//This is the specific person												
												getName(line);
												getSex(line,lineCounter,unmarried);
												if(first_iter->path().filename().string().find("Base")!=std::string::npos)
												{
													related = true;
												}
												else 
												{
													related = isRelated(line);
													if(firstName=="Barbara" &&lastName=="Carter")
													{
														related = true;
													}
													(*allErrors).errorTwo(info,second_iter->path());
												}												
												if( firstName == "Sherry" && lastName == "Taylor" )
												{
													sex = "F";
												}
												getline(file,line);
												getBirth(line);
												(*allErrors).errorOne(line,second_iter->path(),((birthDay+birthMonth+birthYear+deathPlace)==""));
												getline(file,line);
												getDeath(line);
												(*allErrors).errorOne(line,second_iter->path(),((deathDay+deathMonth+deathYear+deathPlace)==""));
												(*allErrors).errorFiveSix(stringtoint(birthYear), stringtoint(deathYear), second_iter->path());
												getline(file,line);											
												parents = getParents(line);
												getMarriage(info);
												(*allErrors).errorNine(stringtoint(marriage), stringtoint(birthYear), stringtoint(deathYear), second_iter->path());
												famS = familyCounter;
												if(related)
												{
													famC = getFamily(parents,p);
												}
												addResources(second_iter->path());
												getRelationship(first_iter, second_iter->path().filename().string(), third_iter);
												break;
											}
										}
									}
								}
							}	
							//Gather info from second family if it exists
							else if((third_iter->path()).filename() == "Info2.txt")
							{
								familyCounter++;
								personCounter2++;
								//If person has not been set yet, check to see if person gets added
								if(famS == 0)
								{
									if(personCounter2 >= personCounter)
									{
										//This is where the person is
										info2 = true;
										location = second_iter->path();
										fs::path info = third_iter->path();										
										(*allErrors).errorFour(second_iter);
										fs::ifstream file(info);
										std::string line;
										int personCounter3 = 0;
										int lineCounter = 0;
										while(getline(file,line))
										{
											if(line.empty()) continue;
											lineCounter++;
											if(isalpha(line.at(0))||line.at(0)=='?')
											{
												personCounter3++;										
												if(line.find(".")!=std::string::npos)
												{
													//This is the specific person
													getName(line);
													getSex(line,lineCounter,unmarried);
													related = false;
													getline(file,line);
													getBirth(line);
													(*allErrors).errorOne(line,second_iter->path(),((birthDay+birthMonth+birthYear+deathPlace)==""));
													getline(file,line);
													getDeath(line);
													(*allErrors).errorOne(line,second_iter->path(),((deathDay+deathMonth+deathYear+deathPlace)==""));
													(*allErrors).errorFiveSix(stringtoint(birthYear), stringtoint(deathYear), second_iter->path());
													getline(file,line);
													parents = getParents(line);
													famS = familyCounter;
													addResources(second_iter->path());
													getRelationship(first_iter, second_iter->path().filename().string(), third_iter);
													return;
												}
											}
										}
									}									
								}	
								else
								{
									//Only set the person whose name checks out
									fs::path info = third_iter->path();										
									(*allErrors).errorFour(second_iter);
									fs::ifstream file(info);
									std::string line;
									int lineCounter = 0;
									while(getline(file,line))
									{
										if(line.empty()) continue;
										lineCounter++;
										if(isalpha(line.at(0))||line.at(0)=='?')
										{										
											if(line.find(".")==std::string::npos)
											{
												//This is the specific person
												std::string oldFirst = firstName;
												std::string oldMiddle = middleName;
												std::string oldLast = lastName;
												firstName = "";
												middleName = "";
												lastName = "";
												getName(line);
												if(firstName != oldFirst)
												{
													firstName = oldFirst;
													middleName = oldMiddle;
													lastName = oldLast;
													return;
												}
												famS2 = familyCounter;
												return;
											}
										}
									}
								}								
							}
						}
						if(famC+famS>0)
							return;
					}
				}
			}
			std::cout<<"Done gathering info for "<<personCounter<<"\n";
		}		
		int countPeople(fs::path info)
		{
			fs::ifstream file(info);
			int personCounter = 0;
			std::string line;
			while(getline(file,line))
			{
				if(line.size()!=0)
				{
					if(isalpha(line.at(0)))
					{
						personCounter++;
					}
				}
			}
			return personCounter;
		}
		void getRelationship(fs::directory_iterator first_iter, std::string what, fs::directory_iterator third_iter )
		{
			std::ostringstream relationship_stream;
			if(first_iter->path().filename() == "1. Base")
			{
				//Male or female?
				if(what.find("Unmarried")!=std::string::npos)
				{
					relationship = "Root";
					return;
				}
				std::string fatherMother = "";
				//Father if sex is "M" otherwise mother
				if(sex == "M")
				{
					fatherMother = "father";
					if(third_iter->path().filename() == "Info2.txt")
					{
						fatherMother = "mother";
					}
				}
				else
				{
					fatherMother = "mother";
					if(third_iter->path().filename() == "Info2.txt")
					{
						fatherMother = "father";
					}
				}
				//How deep?
				if(what.at(0) == '1')
				{
					fatherMother[0] = std::toupper(fatherMother[0]); 
					relationship_stream<<fatherMother; 
				}
				else if (what.at(0) == '2')
					relationship_stream<<"Grand"<<fatherMother;
				else if (what.at(0) == '3')
					relationship_stream<<"Great grand"<<fatherMother;
				else if (what.at(0) == '4')
					relationship_stream<<"2nd great grand"<<fatherMother;
				else if (what.at(0) == '5')
					relationship_stream<<"3rd great grand"<<fatherMother;
				else
				{
					int depth = (int)(what.at(0))-50;
					relationship_stream<<depth<<"th great grand"<<fatherMother;
				}
				if(firstName == "William" && lastName == "Parks") 
				{
					relationship = "Adopted 2nd Great Grandfather";	
					return;
				}
				if(firstName == "Mary" && lastName == "Johnston") 
				{
					relationship = "Adopted 2nd Great Grandfather";	
					return;
				}
				if(third_iter->path().filename() == "Info2.txt")
				{
					std::string temp = relationship_stream.str();
					relationship_stream.str("");
					relationship_stream<<"Married to "<<temp;
				}
			}
			else if(first_iter->path().filename() == "2. Aunts and Uncles")
			{
				//If negative then it's a niece/nephew
				if(what.at(0) == '-')
				{
					//Male or female?
					std::string nieceNephew = "";
					std::string marriedTo = "";
					//Male of female?
					if(sex == "M")
					{
						nieceNephew = "Nephew";
					}
					else
					{
						nieceNephew = "Niece";
					}
					//Married to?
					if(!related)
					{
						marriedTo = " by marriage";
					}
					//How deep?
					if(what.at(1) == '1')
						relationship_stream<<nieceNephew<<marriedTo;
					else if(what.at(1) == '2')
						relationship_stream<<"Great "<<nieceNephew<<marriedTo;
					else if(what.at(1) == '3')
						relationship_stream<<"2nd Great "<<nieceNephew<<marriedTo;
					else if(what.at(1) == '4')
						relationship_stream<<"3rd Great "<<nieceNephew<<marriedTo;
					else
					{
						int depth = (int)(what.at(1))-49;
						relationship_stream<<depth<<"th Great "<<nieceNephew<<marriedTo;
					}
					//What about half nieces and nephews?? Does the logic still work??
				}
				else
				{
					//Male or female?
					std::string auntUncle = "";
					std::string marriedTo = "";
					//Male or female?
					if(sex == "M")
					{
						auntUncle = "Uncle";
					}
					else
					{
						auntUncle = "Aunt";
					}
					//Married to?
					if(!related)
					{
						marriedTo = " by marriage";
					}
					//How deep?
					if(what.at(0) == '1')
						relationship_stream<<auntUncle<<marriedTo;
					else if (what.at(0) == '2')
						relationship_stream<<"Great "<<auntUncle<<marriedTo;
					else if (what.at(0) == '3')
						relationship_stream<<"2nd Great "<<auntUncle<<marriedTo;
					else if (what.at(0) == '4')
						relationship_stream<<"3rd Great "<<auntUncle<<marriedTo;
					else
					{
						int depth = (int)(what.at(0))-49;
						relationship_stream<<depth<<"th Great "<<auntUncle<<marriedTo;
					}
					if(half)
					{
						std::string temp = relationship_stream.str();
						relationship_stream.str("");
						relationship_stream<<"Half "<<temp;
					}
				}
			}
			else if(first_iter->path().filename() == "3. First Cousins")
			{
				//Married to?
				std::string marriedTo = "";
				if(!related)
				{
					marriedTo = "Married to ";
				}
				//How deep?
				if(what.at(0) == '0')
				{
					relationship_stream << marriedTo<<"First Cousin";
				}
				else if(what.at(0) == '1')
				{
					relationship_stream << marriedTo<<"First Cousin Once Removed";
				}
				else if(what.at(0) == '2')
				{
					relationship_stream << marriedTo<<"First Cousin Twice Removed";
				}
				else
				{
					relationship_stream << marriedTo<<"First Cousin " << what.at(0) <<" Times Removed";
				}
				if(half)
				{
					std::string temp = relationship_stream.str();
					relationship_stream.str("");
					relationship_stream<<"Half "<<temp;
				}
			}
			else if(first_iter->path().filename() == "4. Second Cousins")
			{
				//Married to?
				std::string marriedTo = "";
				if(!related)
				{
					marriedTo = "Married to ";
				}
				//How deep?
				if(what.at(0) == '0')
				{
					relationship_stream << marriedTo<<"Second Cousin";
				}
				else if(what.at(0) == '1')
				{
					relationship_stream << marriedTo<<"Second Cousin Once Removed";
				}
				else if(what.at(0) == '2')
				{
					relationship_stream << marriedTo<<"Second Cousin Twice Removed";
				}
				else
				{
					relationship_stream << marriedTo<<"Second Cousin " << what.at(0) <<" Times Removed";
				}	
				if(half)
				{
					std::string temp = relationship_stream.str();
					relationship_stream.str("");
					relationship_stream<<"Half "<<temp;
				}
			}
			else if(first_iter->path().filename() == "5. Third Cousins")
			{
				//Married to?
				std::string marriedTo = "";
				if(!related)
				{
					marriedTo = "Married to ";
				}
				//How deep?
				if(what.at(0) == '0')
				{
					relationship_stream << marriedTo<<"Third Cousin";
				}
				else if(what.at(0) == '1')
				{
					relationship_stream << marriedTo<<"Third Cousin Once Removed";
				}
				else if(what.at(0) == '2')
				{
					relationship_stream << marriedTo<<"Third Cousin Twice Removed";
				}
				else
				{
					relationship_stream << marriedTo<<"Third Cousin " << what.at(0) <<" Times Removed";
				}
				if(half)
				{
					std::string temp = relationship_stream.str();
					relationship_stream.str("");
					relationship_stream<<"Half "<<temp;
				}
			}
			else if(first_iter->path().filename() == "6. Fourth Cousins")
			{
				//Married to?
				std::string marriedTo = "";
				if(!related)
				{
					marriedTo = "Married to ";
				}
				//How deep?
				if(what.at(0) == '0')
				{
					relationship_stream << marriedTo<<"Fourth Cousin";
				}
				else if(what.at(0) == '1')
				{
					relationship_stream << marriedTo<<"Fourth Cousin Once Removed";
				}
				else if(what.at(0) == '2')
				{
					relationship_stream << marriedTo<<"Fourth Cousin Twice Removed";
				}
				else
				{
					relationship_stream << marriedTo<<"Fourth Cousin " << what.at(0) <<" Times Removed";
				}
				if(half)
				{
					std::string temp = relationship_stream.str();
					relationship_stream.str("");
					relationship_stream<<"Half "<<temp;
				}
			}
			relationship = relationship_stream.str();
		}
		//------------------------------------------------------------------------
	private:
		//---------------------------------Functions------------------------------
		bool isRelated(std::string line)
		{
			if(line.find('.')!=std::string::npos)
			{
				return false;
			}
			return true;
		}
		std::string getParents(std::string line_original)
		{
			std::istringstream line(line_original);
			std::string parents = "";
			std::string next = "";
			while(line>>next)
			{
				if(next != "Parents:")
				{
					parents = parents + " " + next;
				}
			}
			parents.erase(0,1);
			return parents;
		}
		void getName(std::string line_original)
		{
			std::istringstream line(line_original);
			std::string next = "";
			while(line >> next)
			{			
				if(firstName == "")
				{
					firstName = next;
				}
				else
				{
					middleName = middleName + " " + lastName;
					lastName = next;
				}
			}
			while(!middleName.empty())
			{
				if(middleName.at(0) == ' ')
				{
					middleName.erase(0,1);
				}
				else break;
			}
			if(lastName.find('.')!=std::string::npos)
			{
				lastName.pop_back();
			}
		}
		void getSex(std::string line, int lineNumber, bool unmarried)
		{
			if(!unmarried)
			{
				if(lineNumber == 1) sex = "M";
				else sex = "F";
			}
			else if(line.find('.')!=std::string::npos) 
			{
				sex = "F";
			}
			else sex = "M";
		}
		void getBirth(std::string line_original)
		{
			std::istringstream line(line_original);
			std::string next = "";
			while(line >> next)
			{
				if(next=="-")
				{
					//Get birth place
					while(line >> next)
					{
						birthPlace = birthPlace + " " + next;
					}
					birthPlace.erase(0,1);
					break;
				}
				birthMonth = birthDay;
				birthDay = birthYear;
				birthYear = next;
			}
			if(birthYear == "Born:")
			{
				birthYear = "";
			}
			else if(birthDay == "Born:")
			{
				birthDay = "";
			}
			else if(birthMonth == "Born:")
			{
				birthMonth = birthDay;
				birthDay = "";
			}
		}
		void getDeath(std::string line_original)
		{
			std::istringstream line(line_original);
			std::string next = "";
			while(line >> next)
			{
				if(next=="-")
				{
					//Get cemetery
					while(line >> next)
					{
						deathPlace = deathPlace + " " + next;
					}
					deathPlace.erase(0,1);
					break;
				}
				deathMonth = deathDay;
				deathDay = deathYear;
				deathYear = next;
			}
			if(deathYear == "Died:")
			{
				deathYear = "";
			}
			else if(deathDay == "Died:")
			{
				deathDay = "";
			}
			else if(deathMonth == "Died:")
			{
				deathMonth = deathDay;
				deathDay = "";
			}
		}
		void getMarriage(fs::path info)
		{
			fs::ifstream file(info);
			std::string line_original = "";
			while(getline(file,line_original))
			{
				if(line_original.find("Married:")!=std::string::npos)
				{
					break;
				}
			}			
			std::istringstream line(line_original);
			std::string next = "";
			while(line >> next)
			{
				if(next=="Married:")
				{
					marriage = "";
				}
				else
				{
					marriage = marriage + " " + next;
				}
			}
			marriage.erase(0,1);
		}
		int getFamily(std::string parents,fs::path p)
		{
			if(parents == "")
			{
				return 0;
			}
			int familyCounter = 0;	
			isHalf(parents);
			if(parents.find("+")!=std::string::npos || parents.find("^")!=std::string::npos)
			{
				familyCounter++;
				parents.pop_back();
			}
			if(parents.find("*")!=std::string::npos)
				parents.pop_back();	
			fs::directory_iterator end_iter;
			//Look at everything in the top folder
			for( fs::directory_iterator first_iter(p) ; first_iter != end_iter ; ++first_iter)
			{
				//Only proceed if current item is a folder
				if(is_directory(first_iter->path()))
				{
					//Look at every folder in this folder
					for( fs::directory_iterator second_iter(first_iter->path()) ; second_iter != end_iter ; ++second_iter)
					{
						if(second_iter->path().string().find("Unmarried") == std::string::npos)
						{
							familyCounter++;
							if(second_iter->path().filename().string().find(parents)!=std::string::npos)
							{
								return familyCounter;
							}
							for(fs::directory_iterator third_iter(second_iter->path()); third_iter != end_iter; ++third_iter)
							{
								if(third_iter->path().filename() == "Info2.txt")
								{
									familyCounter++;
								}
							}
						}
					}
				}
			}			
		}
		bool isHalf(std::string parents)
		{
			if(parents.find("*")!=std::string::npos || parents.find("+")!=std::string::npos )
			{
				return true;
			}
			return false;
		}
		void addResources(fs::path second_iter)
		{
			fs::directory_iterator end_iter;
			for( fs::directory_iterator third_iter(second_iter); third_iter != end_iter; ++third_iter)
			{
				//Look for docs and photos folders
				//Are resources different than sources?
				if(is_directory(third_iter->path()))
				{
					if(third_iter->path().filename() == "Photos" || third_iter->path().filename() == "Docs")
					{						
						for(fs::directory_iterator fourth_iter(third_iter->path()); fourth_iter != end_iter; ++fourth_iter)
						{
							//Add all files as resources
							resourcesFile.push(fourth_iter->path().string());
							std::string form = fourth_iter->path().extension().string();
							form.erase(0,1);
							resourcesForm.push(form);
							std::string title = fourth_iter->path().filename().string();							
							while(title.back() != '.')
							{
								title.pop_back();
							}
							title.pop_back();
							resourcesTitle.push(title);
						}
					}
				}
			}
		}
		//------------------------------------------------------------------------
};
