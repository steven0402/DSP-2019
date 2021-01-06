#include <stdio.h>
#include <stdlib.h>
#include "Ngram.h"
#include "Vocab.h"
#include "File.h"
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>

using namespace std;

double getProb(string word1, string word2, Vocab &vocab, Ngram &lm){

    VocabIndex wordid1 = vocab.getIndex(word1.c_str());
    VocabIndex wordid2 = vocab.getIndex(word2.c_str());

    if(wordid1 == Vocab_None)
        wordid1 = vocab.getIndex(Vocab_Unknown);
    if(wordid2 == Vocab_None)
        wordid2 = vocab.getIndex(Vocab_Unknown);

    VocabIndex context[] = { wordid1, Vocab_None };
    return lm.wordProb( wordid2, context);
}

int main (int argc, char* argv[]) {
	
	char* segment_file = argv[1];
	char* map_file = argv[2];
	char* language_model = argv[3];
	char* output_file = argv[4];
	
	Vocab vocab;
	Ngram lm(vocab, 2);

    File Lmfp(language_model, "r"); 
    lm.read(Lmfp);
    Lmfp.close();
	
	map<string, set<string> > mapping;
    fstream mapfp;
	mapfp.open(map_file, ios::in);
    
	char buff[100000];
	vector<string> textline;
	fstream testfp(segment_file, ios::in);
	
	int pos = 0;
	
	char map_buff[2];
	char map_next_line;
	string word;

	fstream outfp;
	outfp.open(output_file, ios::out);
	
	//read map
	while(mapfp.read(map_buff,2)){

		word = string(map_buff);
		mapping[word] = {};
		mapfp.get(map_next_line);
		while(map_next_line != '\n'){
			mapfp.read(map_buff,2);

			mapping[word].insert(string(map_buff));
			mapfp.get(map_next_line);
		}
	}
	/*
	for(map<string, set<string> >::const_iterator it = mapping.begin();
		it != mapping.end(); ++it)
	{
		cout << it->first << ": " ;
		for(set<string> ::const_iterator itt = it->second.begin();
			itt != it->second.end(); ++itt)
		cout << *itt << " " ;
		cout << endl;
	}*/
	
	// read test file
	while(testfp.getline(buff,sizeof(buff))){
		
		string temp = string(buff);
		textline.push_back("<s>");
		temp.erase(remove(temp.begin(), temp.end(), ' '), temp.end());
		for(int i = 0; i < temp.size() ; i+=2){
			textline.push_back(temp.substr(i, 2));
		}
		textline.push_back("</s>");
		/*
		for (vector<string>::const_iterator i = textline.begin(); i != textline.end(); ++i)
			cout << *i << ' ';
		
		cout << endl;
		*/
		map<string, set<string> >::iterator map_it;
		vector<vector<string>> wordTable;
		
		for(vector<string>::const_iterator it = textline.begin();
			it != textline.end(); ++it)
		{
			map_it = mapping.find(*it);
			vector<string> tempWord;
			
			if (map_it == mapping.end())
				tempWord.push_back(*it);
			else
				for (set<string>::const_iterator ait = map_it->second.begin();
					ait != map_it->second.end(); ++ait){
						tempWord.push_back(*ait);
					}
			
			wordTable.push_back(tempWord);

		}
		
		vector<vector<double>> prob_list;
		vector<vector<int>> previous_word_index;
		for(int i = 0; i < wordTable.size(); i++){
			vector<int> temp_word_index;
			vector<double> temp_prob;
			if(i == 0)
			{
				vector<double> temp_prob;
				for(int j = 0; j < wordTable.at(0).size(); j++)
				{
					temp_prob.push_back(0.0);
				}
				prob_list.push_back(temp_prob);
			}
			else
			{
				for(int j = 0; j < wordTable.at(i).size(); j++)
				{
					double maxProb = -1000.0;
					double nowProb = 0.0;
					int maxIndex = 0;

					for(int k = 0; k < wordTable.at(i-1).size(); k++)
					{
						nowProb = getProb(wordTable.at(i-1).at(k), wordTable.at(i).at(j), vocab, lm);
						if (nowProb + prob_list.at(i-1).at(k) > maxProb){
							maxIndex = k;
							maxProb = nowProb + prob_list.at(i-1).at(k);
						}

					}
					temp_prob.push_back(maxProb);
					temp_word_index.push_back(maxIndex);
					
				}
				prob_list.push_back(temp_prob);
				previous_word_index.push_back(temp_word_index);

			}
		}
		int startIndex = -1;
		int max_final = -1000.0;
		vector<int> word_seq;
		
		for(int i = 0; i < wordTable.at(wordTable.size()-1).size(); i++){
			if (prob_list.at(wordTable.size()-1).at(i) > max_final){
				startIndex = i;
				max_final = prob_list.at(wordTable.size()-1).at(i);
			}
		}
		word_seq.push_back(startIndex);
		for(int i = previous_word_index.size()-1; i >= 0; i--)
		{
				word_seq.push_back(previous_word_index.at(i).at(startIndex));
				startIndex = previous_word_index.at(i).at(startIndex);
		}
		int wordTable_temp = 0;
		for(int i = word_seq.size()-1; i >= 0; i--)
		{
				outfp << wordTable.at(wordTable_temp).at(word_seq.at(i));
				if (i == 0)
					outfp << endl;
				else
					outfp << " ";
				wordTable_temp ++;
		}

		
		

		for(int i = 0; i < 100000; i++){
			buff[i] = '\0';
		}
		textline.clear();
	}
	outfp.close();
	
}