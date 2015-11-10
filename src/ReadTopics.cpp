#include "indri/ReadTopics.hpp"

map< string, map <string, double> > ReadTopics::docsTopics = map< string, map <string, double> >();
map< string, map <string, double> > ReadTopics::topicsWords = map< string, map <string, double> >();
map< size_t, string > ReadTopics::documentIdName = map< size_t, string >();
double ReadTopics::lambda = 0.0;

	ReadTopics::ReadTopics(string docTopicsFileName, string topicsWordsFileName, string repName, indri::api::Parameters& _params)
: params(_params)
{
	genMapdocumentIdName(repName);
	readDocsTopics(docTopicsFileName); 
	readTopicsWords(topicsWordsFileName);
	if(!params.exists("intCoeff"))
		throw runtime_error("intCoeff does not exist in the params file.");
	lambda = params.get("intCoeff", 0.0);
}
// to calculate the probabilities of a word given a document
double ReadTopics::calcProb(string word, size_t docId)
{
	//std::cout << "calculating Probability... " << std::endl;
	double probWordDoc = 0;

	string documentName = documentIdName[docId];
	std::cout << "document ID = " << docId << std::endl;
	std::cout << "documentName = " << documentName << std::endl;

	map< string, map <string, double> >::iterator itFoundDoc = docsTopics.find(documentName); // find topics of a document
	map <string, double> oneDocsTopics = itFoundDoc->second;
	for (map <string, double>::iterator itTopics = oneDocsTopics.begin(); itTopics != oneDocsTopics.end(); itTopics++) // for each topic
	{
		string topicName = itTopics->first;
		double probDocTopic = itTopics->second;
		std::cout << "\ttopicName = " << topicName << std::endl;;
		std::cout << "\tprobDocTopic = " << probDocTopic << std::endl;
		map< string, map <string, double> >::iterator itFoundWords = topicsWords.find(topicName); // find word in that topic
		map <string, double> oneTopicWords = itFoundWords->second;

		map <string, double>::iterator itFoundWord = oneTopicWords.find(word);
		
		if (itFoundWord != oneTopicWords.end())
		{
			double probTopicWord = itFoundWord->second;
			probWordDoc += probTopicWord * probDocTopic;

			std::cout << "\t\twordName = " << itFoundWord->first << std::endl;;
			std::cout << "\t\tprobTopicWord = " << probTopicWord << std::endl;;
			std::cout << "\t\tprobWordDoc = " << probWordDoc << std::endl;
		}
	}
	std::cout << "probability calculated... probWordDoc = " << probWordDoc << std::endl;
	return probWordDoc;
}
void ReadTopics::readDocsTopics(string fileName) 
{
	docsTopics = readFile(fileName);
	//for(map< string, map <string, double> >::iterator it = docsTopics.begin(); it != docsTopics.end(); it++ )
	//{
	//	cout << "document ID = " << it->first << endl;
	//	for(map <string, double>::iterator it2 = (it->second).begin(); it2 != (it->second).end(); it2++)
	//		cout << "topic ID = " << it2->first << " prob = " << it2->second << endl;
	//}

	std::cout << "size of docsTopics = " << docsTopics.size() << std::endl;
}
void ReadTopics::readTopicsWords(string fileName) 
{
	topicsWords = readFile(fileName);
	std::cout << "size of topicsWords = " << topicsWords.size() << std::endl;
}

map< string, map <string, double> > ReadTopics::readFile(string fileName)
{
	std::cout << "reading topic files..." << std::endl;
	std::cout << "file Name = " << fileName << std::endl;
	map< string, map <string, double> > topicsMap; 
	ifstream file(fileName.c_str());
	if(!file.good())
		throw runtime_error(fileName + " is not readable");
	string line;
	string oneColumn = ""; 
	string twoColumn1 = "";
	double twoColumn2 = 0.0;
	while(getline(file, line))
	{
		//cout << "line = " << line << endl;
		std::vector<std::string> strs;
		boost::split(strs, line, boost::is_any_of("\t "));
		if(strs.size() == 1) // if there is only one column in the line
		{
			oneColumn = strs[0];
		}
		else if (strs.size() == 2)
		{
			twoColumn1 = strs[0];
			twoColumn2 = atof(strs[1].c_str());
		}
		else
		{
			throw runtime_error("there should be at most 2 columns per row in lda graph files");
		}
		//cout << "oneColumn = " << oneColumn << " twoColumn1 = " << twoColumn1 << " twoColumn2 = " << twoColumn2 << endl;
		if(oneColumn != "" && twoColumn1 != "")
			topicsMap[oneColumn].insert(make_pair(twoColumn1, twoColumn2));		
	}
	cout << "size of topicsMap = " << topicsMap.size() << endl;
	return topicsMap;
}

void ReadTopics::genMapdocumentIdName(string repName)
{
	indri::api::QueryEnvironment env;
	env.addIndex( repName );
	indri::collection::Repository r;
	r.openRead( repName );
	indri::collection::CompressedCollection* collection = r.collection();
	for (size_t i = 1; i <= env.documentCount(); i++) // for all the document IDs;
	// document IDs start from 1 to "documentCount"
	{
		std::string documentName = collection->retrieveMetadatum( i, "docno" );
		documentIdName.insert(make_pair(i, documentName));
	}
	r.close();
	env.close();

	//std::cout << "size of documentIdName = " << documentIdName.size() << std::endl;
}

double ReadTopics::getLambda()
{
	return lambda;
}

