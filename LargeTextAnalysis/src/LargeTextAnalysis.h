// LargeTextAnalysis.h 
#pragma once
#ifndef LARGE_TEXT_ANALYSIS_H
#define LARGE_TEXT_ANALYSIS_H

#include <iostream>
#include <unordered_map>
#include <fstream>
#include <string>
#include <algorithm>
#include <thread>
#include <filesystem>
#include "bcppul/timer.h"
#include "bcppul/file_utils.h"
#include "bcppul/string_utils.h"

namespace bulka
{
	char* delimiters = new char[] {" .,:–—1234567890(){};\"\'`[]/«»!?„“…№#$@*\n\t\r"};
	bool comparator(const std::pair<std::string, unsigned int>& a, const std::pair<std::string, unsigned int>& b);
	bool comparator_char(const std::pair<char, long long>& a, const std::pair<char, long long>& b);

	class VLT {
	private:
	protected:
		std::string path = "";
		std::string outFile = "";
		std::unordered_map<std::string, unsigned int> words;
		std::vector<std::pair<std::string, unsigned int>> vec_words;
		long long letters[256]{};
		long long words_count = 0;
		long long lines_count = 0;
		long long length = 0;
		bcppul::Timer timer;
	public:
		VLT(const std::string path = "", const std::string outFile = "");
		~VLT();
		void load();
		void analys();
		friend std::ostream& operator<< (std::ostream& out, VLT& vlt);
		void printResult();
		void writeResultInFile();
		void writeResultInFile(bool append);
		const std::string& getPath() const;
		void setPath(std::string& path);
		const std::string& getOutFile() const;
		void setOutFile(std::string& outFile);
		std::unordered_map<std::string, unsigned int>& getWords();
		std::vector<std::pair<std::string, unsigned int>>& getVectorWords();
		long long* getLetters();
		bcppul::Timer& getTimer();
		long long getWordsCount();
		long long getLinesCount();
		long long getLength();
	};

	class Texts {
	private:
	protected:
		std::string outFile = "";
		std::vector <bulka::VLT> texts;
		std::unordered_map<std::string, unsigned int> words_sum;
		std::vector<std::pair<std::string, unsigned int>> vec_words_sum;
		bool summary = true;
		bool each = true;
		long long letters_sum[256]{};
		long long words_count_sum = 0;
		long long lines_count_sum = 0;
		long long length_sum = 0;
		bcppul::Timer timer;
	public:
		Texts(bool summary = true, bool each = true, const std::string outFile = "");
		Texts(std::vector<std::string> paths, bool summary = true, bool each = true, const std::string outFile = "");
		~Texts();
		void singleTextAnalys(bulka::VLT& text);
		void analys();
		friend std::ostream& operator<< (std::ostream& out, Texts& texts);
		void printResultSum();
		void printResultAll();
		void writeResultSumInFile();
		void writeResultAllInFile();
		void updatePaths(std::vector<const char*> paths);
		void updatePaths(std::vector<std::string> paths);
		std::unordered_map<std::string, unsigned int>& getWordsSum();
		std::vector<std::pair<std::string, unsigned int>>& getVectorWordsSum();
		long long* getLettersSum();
		bcppul::Timer& getTimer();
		long long getWordsCountSum();
		long long getLinesCountSum();
		long long getLengthSum();
		bool isSummary() const;
		void setSummary(bool summary);
		bool isEach() const;
		void setEach(bool summary);
		const std::string& getOutFile() const;
		void setOutFile(std::string& outFile);
	};
	void parseArguments(int argc, char* argv[], bool& recursively, bool& summary, bool& print, std::string& outFile, std::vector<std::string>& files, bool& each);
	void largeTextAnalysis(int argc, char* argv[]);
} //namespace bulka


#endif

