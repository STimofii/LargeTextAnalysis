﻿// LargeTextAnalysis.h 
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
	extern char* delimiters;
	bool comparator(const std::pair<std::string, unsigned long long>& a, const std::pair<std::string, unsigned long long>& b);
	bool comparator_char(const std::pair<char, unsigned long long>& a, const std::pair<char, unsigned long long>& b);

	class VLT {
	private:
	protected:
		std::string path = "";
		std::string outFile = "";
		std::unordered_map<std::string, unsigned long long> words;
		unsigned long long letters[256]{};
		unsigned long long words_count = 0;
		unsigned long long lines_count = 0;
		unsigned long long text_lines_count = 0;
		unsigned long long length = 0;
		bool words_stat = true;
		bool chars_stat = true;
		bcppul::Timer timer;
	public:
		VLT(const std::string path = "", bool words_stat = true, bool chars_stat = true, const std::string outFile = "");
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
		std::unordered_map<std::string, unsigned long long>& getWords();
		unsigned long long* getLetters();
		bcppul::Timer& getTimer();
		unsigned long long getWordsCount() const;
		unsigned long long getLinesCount() const;
		unsigned long long getTextLinesCount() const;
		unsigned long long getLength() const;
		bool isWordsStat() const;
		void setWordsStat(bool summary);
		bool isCharsStat() const;
		void setCharsStat(bool summary);
	};

	class Texts {
	private:
	protected:
		std::string outFile = "";
		std::vector <bulka::VLT> texts;
		std::unordered_map<std::string, unsigned long long> words_sum;
		bool summary = true;
		bool each = true;
		bool words_stat = true;
		bool chars_stat = true;
		unsigned long long letters_sum[256]{};
		unsigned long long words_count_sum = 0;
		unsigned long long lines_count_sum = 0;
		unsigned long long text_lines_count_sum = 0;
		unsigned long long length_sum = 0;
		bcppul::Timer timer;
	public:
		Texts(bool summary = true, bool each = true, bool words_stat = true, bool chars_stat = true, const std::string outFile = "");
		Texts(std::vector<std::string> paths, bool summary = true, bool each = true, bool words_stat = true, bool chars_stat = true, const std::string outFile = "");
		~Texts();
		void singleTextAnalys(bulka::VLT& text);
		void analys();
		friend std::ostream& operator<< (std::ostream& out, Texts& texts);
		void printResultSum();
		void printResultAll();
		void writeResultSumInFile();
		void writeResultAllInFile();
		void updatePaths(std::vector<std::string> paths);
		std::unordered_map<std::string, unsigned long long>& getWordsSum();
		unsigned long long* getLettersSum();
		bcppul::Timer& getTimer();
		unsigned long long getWordsCountSum() const;
		unsigned long long getLinesCountSum() const;
		unsigned long long getTextLinesCountSum() const;
		unsigned long long getLengthSum() const;
		bool isSummary() const;
		void setSummary(bool summary);
		bool isEach() const;
		void setEach(bool summary);
		bool isWordsStat() const;
		void setWordsStat(bool summary);
		bool isCharsStat() const;
		void setCharsStat(bool summary);
		const std::string& getOutFile() const;
		void setOutFile(std::string& outFile);
	};
	bool parseArguments(int argc, char* argv[], bool& recursively, bool& summary, bool& print, std::string& outFile, std::vector<std::string>& files, bool& each, bool& words_stat, bool& chars_stat);
	void largeTextAnalysis(int argc, char* argv[]);
} //namespace bulka


#endif

