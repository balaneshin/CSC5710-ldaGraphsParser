#ifndef READTOPICS_HPP
#define READTOPICS_HPP

#include<iostream>
#include<map>
#include<vector>
#include<string>
#include<fstream>
#include <boost/algorithm/string.hpp>
#include <stdexcept>

#include "indri/Repository.hpp"
#include "indri/CompressedCollection.hpp"
#include "indri/LocalQueryServer.hpp"
#include "indri/ScopedLock.hpp"
#include "indri/QueryEnvironment.hpp"

using namespace std;

class ReadTopics
{
public:
	ReadTopics(){};
    ReadTopics(string docTopicsFileName, string topicsWordsFileName, string repName, indri::api::Parameters& _params);
	// to calculate the probabilities of a word given a document
    double calcProb(string word, size_t docId);
	void readDocsTopics(string fileName); 
	void readTopicsWords(string fileName); 
    double getLambda(); 
private:
	static map< string, map <string, double> > docsTopics;
	static map< string, map <string, double> > topicsWords;
	static map< size_t, string > documentIdName;
	static double lambda;
    indri::api::Parameters params;

private:
	map< string, map <string, double> > readFile(string fileName);
	// generate a map of document IDs to document Names
	// reason: we have our lda graphs in terms of document names, but we need
	// them to be in terms of document IDs
    void genMapdocumentIdName(string repName);
};



#endif
