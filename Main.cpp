#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <map>
#include <list>
#include <regex>

 // // // // // // // // // // // // // // // // // // 

 // FUNCTIONS
	
	//GET LINKS
	std::list<std::string> extractLinksFromHTML(const std::string& fileContent) 
	{
	    std::list<std::string> links;
	    // regular expression to match href attributes in anchor tags
	    std::regex linkRegex("<a\\s+[^>]*href\\s*=\\s*['\"]([^'\"]+)['\"][^>]*>");
	    std::smatch match;

	    // search for links in the HTML content
	    std::string::const_iterator start = fileContent.cbegin();
	    while (std::regex_search(start, fileContent.cend(), match, linkRegex)) {
	        if (match.size() > 1) {
	            links.push_back(match[1].str());
	        }
	        start = match.suffix().first;
	    }
	    return links;
	}

	//LINK SHORTENER
	std::string LinkShorter(std::string Link)
	{
		std::string::iterator StringItr = Link.end();
		std::string TempLink;

		while (*StringItr != '/')
		{
			if (StringItr == Link.begin())
			{
			StringItr--;
			break;
			}
		StringItr--;
		}

		while (*StringItr != '.')
		{
		StringItr++;

			if (*StringItr == '.')
			{
			break;
			}
		
		TempLink = TempLink + *StringItr;
		}

		Link = TempLink;

		return Link;
	}

	//SEARCHES FOR A WORD AND REMOVES IT
	void WordSearcher(std::string& File, std::string& WordTarget, int& Occ)
	{
		unsigned long found = File.find(WordTarget);
		if (found != std::string::npos)
		{
		int WordTargetSize = WordTarget.length();
		std::size_t TargetPos = File.find(WordTarget);
		File.erase(TargetPos-found, TargetPos+WordTargetSize);
		Occ++;
		WordSearcher(File, WordTarget, Occ);
		}
		return;
	}

	//SORTER
	bool comp(const std::vector<std::string>& vec1, const std::vector<std::string>& vec2)
	{
		if (stod(vec1[9]) < stod(vec2[9]))
		{
		return false;
		}
		return true;
	}


	//WEB CRAWLER
	void WebCrawler(std::string& OGPath, std::string NewPath, std::vector<std::string>& Links, std::vector<std::string>& Words, std::vector<std::string>& TargetLink, std::vector<std::vector<std::string> >& TargetLinkStorage, std::map<std::string, double>& Map, std::map<std::string, std::vector<std::string> >& MapLink, std::vector<double>& LengthVector, int INDEX){

		//VARIABLES
		std::list<std::string> File;
		std::string filePath = NewPath;
		std::vector<std::string> UnseenLink;

		//OPENING LINK
		std::ifstream in_str2(filePath);
		if (in_str2.is_open()) 
		{
		std::string fileContent((std::istreambuf_iterator<char>(in_str2)), std::istreambuf_iterator<char>());
		//out_str << fileContent << std::endl;
		File = extractLinksFromHTML(fileContent);

		double FileLength = fileContent.length();
		LengthVector.push_back(FileLength);

		//GET TITLE
		std::size_t TitlePos = fileContent.find("<title>");
		std::size_t TitlePosEnd = fileContent.find("</title>");
		std::string CurrentTitle = fileContent.substr(TitlePos+7, TitlePosEnd-TitlePos-7); 

		//GET DESCRIPTION
		std::size_t DesPos = fileContent.find("content=");
		std::size_t DesPosEnd = fileContent.find("</head>");
		std::string CurrentDes = fileContent.substr(DesPos+9, DesPosEnd-DesPos-12); 

		//GET HEADER
		std::size_t HeadPos = fileContent.find("<h1>");
		std::size_t HeadEndPos = fileContent.find("</h1>");
		std::string CurrentLink = fileContent.substr(HeadPos+4, HeadEndPos-HeadPos-15); 

		//GET CURRENT LINKS
		std::size_t HeadPos2 = fileContent.find("<h1>");
		std::size_t HeadEndPos2 = fileContent.find("</h1>");
		std::string CurrentLink2 = fileContent.substr(HeadPos2+15, HeadEndPos2-HeadPos2-15); 

		//GET BODY
		std::size_t BodyPos = fileContent.find("<body>");
		std::size_t BodyPosEnd = fileContent.find("</body>");
		std::string Body = fileContent.substr(BodyPos+6, BodyPosEnd-BodyPos-6); 

		//ITERATORS
		std::list<std::string>::iterator FileItr = File.begin();

		//SEARCH FOR THE WORD
		int WordsSize = Words.size();
		std::string TEMP_string;

		//IF MULTIPLE WORD SEARCH, COMBINE THEM
		if (WordsSize != 1)
		{
			for (int i = 0; i < WordsSize; i++)
			{
				if (i == WordsSize-1)
				{
				TEMP_string = TEMP_string + Words[i];
				break;
				}
			TEMP_string = TEMP_string + Words[i] + " ";
			}
		}
		else
		{
		TEMP_string = Words[0];
		}

		//SEARCH WITH PHRASE TOGETHER
		bool WordsFound = false;

		unsigned long found = Body.find(TEMP_string);
		if (found != std::string::npos)
    {
    	std::string EX_HEAD = CurrentTitle;
			std::string EX_URL = "html_files/" + CurrentLink2;
			std::string EX_DES = CurrentDes;

			size_t periodPos = Body.rfind(".", found);
			periodPos++;

			bool Space = true;
			while (Space == true)
			{
				std::string TempSpace = Body.substr(periodPos, 1);
				if (std::isspace(TempSpace.at(0)))
				{
				periodPos = periodPos + 1;
				}
				else
				{
				Space = false;
				}
			}

			std::string EX_SNIPPET = Body.substr(periodPos, 120); 

			TargetLink.push_back(EX_HEAD);
			TargetLink.push_back(EX_URL); 
			TargetLink.push_back(EX_DES); 
			TargetLink.push_back(EX_SNIPPET);

			//DENSITY
			int FileLength = fileContent.length();
			int HowManyOcc = 0;
			std::string fileContentTemp = fileContent;
			
			WordSearcher(fileContentTemp, TEMP_string, HowManyOcc);

			//DENSITY IN DOCUMENT
			std::string OccString = std::to_string(HowManyOcc);
			TargetLink.push_back(OccString);

			//TOTAL LENGTH OF DOCUMENT
			std::string FileLengthString = std::to_string(FileLength);
			TargetLink.push_back(FileLengthString);

			TargetLink.push_back(LinkShorter(CurrentLink2));

			//FINISH
			TargetLinkStorage.push_back(TargetLink);
			TargetLink.clear();
			WordsFound = true;
    }
    else
    {
    }


		//SEARCH FOR EACH WORDS SEPERATELY
		if (WordsSize != 1)
		{
			if (WordsFound == false)
			{
				bool AllWordsInDoc = true;
				for (int i = 0; i < WordsSize; i++)
				{
				unsigned long found = Body.find(Words[i]);

				if (found != std::string::npos)
		    {

		    }
		    else
		    {
		    AllWordsInDoc = false;
		    }

				}

				if (AllWordsInDoc == true)
			   {

			   	bool Check = true;
			    unsigned long found;
			    int i = 0;
			    while(Check == true)
					{	
						found = Body.find(Words[i]);
						if (found != std::string::npos)
				    {
				    Check = false;
				    }	
				    else
				    {
				    i++;
				    }
					}

			    	std::string EX_HEAD = CurrentTitle;
						std::string EX_URL = "html_files/" + CurrentLink2;
						std::string EX_DES = CurrentDes;
						
						size_t periodPos = Body.rfind(".", found);
						periodPos++;
						
						bool Space = true;
						while (Space == true)
						{
							std::string TempSpace = Body.substr(periodPos, 1);
							if (std::isspace(TempSpace.at(0)))
							{
							periodPos = periodPos + 1;
							}
							else
							{
							Space = false;
							}
						}

						std::string EX_SNIPPET = Body.substr(periodPos, 120); 

						TargetLink.push_back(EX_HEAD);
						TargetLink.push_back(EX_URL);
						TargetLink.push_back(EX_DES);
						TargetLink.push_back(EX_SNIPPET);

						//DENSITY
						int FileLength = fileContent.length();
						int HowManyOcc = 0;
						std::string fileContentTemp = fileContent;
						
						WordSearcher(fileContentTemp, Words[i], HowManyOcc);

						//DENSITY IN DOCUMENT
						std::string OccString = std::to_string(HowManyOcc);
						TargetLink.push_back(OccString);

						//TOTAL LENGTH OF DOCUMENT
						std::string FileLengthString = std::to_string(FileLength);
						TargetLink.push_back(FileLengthString);

						TargetLink.push_back(LinkShorter(CurrentLink2));

						//FINISH
						TargetLinkStorage.push_back(TargetLink);
						TargetLink.clear();
			    }
			    else
			    {
			    }
				}
			}
		
		//CHECK TO SEE IF LINK IS ALREADY CHECKED
		int SizeOfVector = Links.size();

		bool DoesLinkExist = false;
		for (FileItr->begin(); FileItr != File.end(); FileItr++)
		{
			Map[LinkShorter(CurrentLink2)]++;
			MapLink[LinkShorter(*FileItr)].push_back(LinkShorter(CurrentLink2));

			DoesLinkExist = false;
			for (int i=0; i<SizeOfVector; i++)
			{	
				if (LinkShorter(*FileItr) == Links[i])
				{
				DoesLinkExist = true;
				}
				else
				{			
				}
			}

			if (DoesLinkExist == false)
			{
			UnseenLink.push_back(*FileItr);
			Links.push_back(LinkShorter(*FileItr));	
			}
		}
		

		int SizeOfUnSeenLinks = UnseenLink.size();
		for (int i=0; i<SizeOfUnSeenLinks; i++)
		{	
			WebCrawler(OGPath, CurrentLink + "/" + UnseenLink[i], Links, Words, TargetLink, TargetLinkStorage, Map, MapLink, LengthVector, INDEX);
		}
		}

	}




 // // // // // // // // // // // // // // // // // // 

 // MAIN

int main(int argc, char** argv){
	
	//INPUT
	std::ifstream in_str(argv[1]);

  //OUTPUT
  std::ofstream out_str(argv[2]);

  //SEARCH WORDS
  std::vector<std::string> Words;

  std::string TempString = argv[3];

  for (int i = 3; i < argc; i++)
  {
  Words.push_back(argv[i]);
  }

  int WordsSize = Words.size();

  for (int i = 0; i < WordsSize; i++)
  {

 	if (Words[i][0] == '"')
  {
  Words[i].erase(0, 1);
  }

  int WordsSize2 = Words[i].length();

  if (Words[i][WordsSize2-1] == '"')
  {
  Words[i].erase(WordsSize2-1, -1);
  }

  }



  //LIST & VECTOR & MAP
	std::list<std::string> File;
	std::vector<std::string> LinkAlreadySeen;
	std::vector<std::string> TargetLink;
	std::vector<std::vector<std::string> > TargetLinkStorage;
	std::map<std::string, double> OutgoingLinksMaps;
	std::map<std::string, std::vector<std::string> > LinksMaps;
	std::vector<double> LinkLengths;

	//STRING
	std::string OGfilePath = "html_files";
	std::string filePath;



 // // // // // // // // // // // // // // // // // // 

 // WEBCRAWLER

	filePath = OGfilePath + "/" + "index.html";
	
	LinkAlreadySeen.push_back(LinkShorter(filePath));
	WebCrawler(OGfilePath, filePath, LinkAlreadySeen, Words, TargetLink, TargetLinkStorage, OutgoingLinksMaps, LinksMaps, LinkLengths, 0);



 // // // // // // // // // // // // // // // // // // 

 // PRINTING / TESTING

	int WordSize = Words.size();

	//IF EMPTY
	if (TargetLinkStorage.empty())
	{
	out_str << "Your search - ";

	for (int i = 0; i < WordSize; i++)
	{
	out_str << Words[i];
	out_str << " ";
	}

	out_str << "- did not match any documents." << std::endl;
	exit(1);
	}

	//DENSITY
	int TargetLinkStorageSize = TargetLinkStorage.size();

	double TotalKeyDensity = 0; 
	for (int i = 0; i < TargetLinkStorageSize; i++) 
	{
	TotalKeyDensity = TotalKeyDensity + stod(TargetLinkStorage[i][4]);
	}

	double TotalLengths = 0; 
	int LinkLengthsSize = LinkLengths.size();
	for (int i = 0; i < LinkLengthsSize; i++) 
	{
	TotalLengths = TotalLengths + LinkLengths[i];
	}

	TotalKeyDensity = TotalKeyDensity / TotalLengths;

	//Calculating Key Density Score
	for (int i = 0; i < TargetLinkStorageSize; i++) 
	{
	double Occ = stod(TargetLinkStorage[i][4]);
	double Length = stod(TargetLinkStorage[i][5]);
	double DensityScore = Occ / (Length * TotalKeyDensity);
	TargetLinkStorage[i].push_back(std::to_string(DensityScore));
	}

	std::map<std::string, std::vector<std::string> >::iterator MapsItr2 = LinksMaps.begin();

	for (int o = 0; o < TargetLinkStorageSize; o++)
	{

		double Backlink = 0;
		MapsItr2 = LinksMaps.begin();

		if (TargetLinkStorage[o][6] == "index")
		{
		Backlink = Backlink + 1.0 / 1;
		TargetLinkStorage[o].push_back(std::to_string(Backlink));
		}

		while (MapsItr2 != LinksMaps.end())
		{
			if (MapsItr2->first == TargetLinkStorage[o][6])
			{
				int LinkSize = MapsItr2->second.size();

				for (int i = 0; i < LinkSize; i++)
				{

				std::map<std::string, double>::iterator MapsItr = OutgoingLinksMaps.begin();

					while (MapsItr != OutgoingLinksMaps.end())
					{

					if (MapsItr->first == MapsItr2->second[i])
					{
					Backlink = Backlink + 1.0 / (1 + MapsItr->second);
					break;
					}

					MapsItr++;
					}

				}
				TargetLinkStorage[o].push_back(std::to_string(Backlink));

			}
			MapsItr2++;
		}
	}

	for (int j = 0; j < TargetLinkStorageSize; j++)
	{
	double PageScore = 0;
 		
	PageScore = ((0.5 * stod(TargetLinkStorage[j][7])) + (0.5 * stod(TargetLinkStorage[j][8])));
	TargetLinkStorage[j].push_back(std::to_string(PageScore));
	}

	//Sorting
	if (WordSize == 1)
	{
	std::sort(TargetLinkStorage.begin(), TargetLinkStorage.end(), comp);
	}
	out_str << "Matching documents: " << std::endl;
	out_str << std::endl; 
	for (int i = 0; i < TargetLinkStorageSize; i++) 
	{
		if (i == TargetLinkStorageSize-1)
		{
		int j = 0;
	  out_str << "Title: " << TargetLinkStorage[i][j] << std::endl; 
	  j++;
	  out_str << "URL: " << TargetLinkStorage[i][j] << std::endl;
	  j++;
	  out_str << "Description: " << TargetLinkStorage[i][j] << std::endl;
	  j++;
	  out_str << "Snippet: " << TargetLinkStorage[i][j] << std::endl; 
	  break;
		}

		int j = 0;
	  out_str << "Title: " << TargetLinkStorage[i][j] << std::endl;
	  j++;
	  out_str << "URL: " << TargetLinkStorage[i][j] << std::endl;
	  j++;
	  out_str << "Description: " << TargetLinkStorage[i][j] << std::endl;
	  j++;
	  out_str << "Snippet: " << TargetLinkStorage[i][j] << std::endl;
	  out_str << std::endl;
	}

return 0;
}
