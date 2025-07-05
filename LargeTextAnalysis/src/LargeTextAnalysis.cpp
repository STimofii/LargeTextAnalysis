// LargeTextAnalysis.cpp

#include "LargeTextAnalysis.h"

const char* locale = "ua";

namespace bulka {
	char* delimiters = new char[] {" .,:–—1234567890(){};\"\'`[]/«»!?„“…№#$@*\n\t\r"};

	bool comparator(const std::pair<std::string, unsigned long long>& a, const std::pair<std::string, unsigned long long>& b) {
		if (a.second != b.second) {
			return a.second > b.second;
		}
		return a.first < b.first;
	}
	bool comparator_char(const std::pair<char, unsigned long long>& a, const std::pair<char, unsigned long long>& b) {
		if (a.second != b.second) {
			return a.second > b.second;
		}
		return a.first < b.first;
	}


	VLT::VLT(const std::string path, bool words_stat, bool chars_stat, const std::string outFile) : path(path), words_stat(words_stat), chars_stat(chars_stat), outFile(outFile)
	{

	}
	VLT::~VLT() {

	}


	void VLT::load() {
		std::cout << "Analyzing " << path << std::endl;
		timer.setName(path.c_str());
		timer.start();
		std::ifstream fin;
		//fin.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		//fin.imbue(std::locale(locale));
		fin.open(path);


		if (!fin.is_open()) {
			throw std::exception("FIN IS NOT OPEN!");
		}
		std::string line;
		while (std::getline(fin, line)) {
			++lines_count;
			if (line.size() == 0 || line.empty()) {
				continue;
			}
			++text_lines_count;
			length += line.size();
			for (size_t i = 0; i < line.length(); ++i)
			{
				++letters[static_cast<unsigned char>(line[i])];
			}
			std::transform(line.begin(), line.end(), line.begin(),
				[](unsigned char c) { return std::tolower(c); });
			size_t current_pos = 0;
			while (current_pos < line.length()) {
				size_t word_start = line.find_first_not_of(delimiters, current_pos);

				if (word_start == std::string::npos) {
					break;
				}

				size_t word_end = line.find_first_of(delimiters, word_start);

				std::string word;
				if (word_end == std::string::npos) {
					word = line.substr(word_start);
					current_pos = line.length();
				}
				else {
					word = line.substr(word_start, word_end - word_start);
					current_pos = word_end;
				}

				if (!word.empty()) {
					words[word] += 1;
					++words_count;
				}
			}
		}


		fin.close();

		timer.stop();
		std::cout << "Analyzed " << path << "\nElapsed time - " << timer.getTimeSeconds() << "s" << std::endl;
	}

	void  VLT::analys() {
		try {
			load();
		}
		catch (std::exception& exception) {
			std::cerr << "Error in text analys: " << exception.what() << std::endl;
		}


	}


	void  VLT::printResult() {
		std::cout << *this << std::endl;
	}
	void  VLT::writeResultInFile() {
		writeResultInFile(false);
	}
	void  VLT::writeResultInFile(bool append) {
		if (outFile.empty()) {
			return;
		}
		std::ofstream fout;
		fout.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		//fout.imbue(std::locale(locale));
		if (append)
		{
			fout.open(outFile, std::fstream::ios_base::app);
		}
		else
		{
			fout.open(outFile);
		}

		if (!fout.is_open()) {
			throw std::exception("FOUT IS NOT OPEN!");
		}

		fout.imbue(std::locale("C"));
		fout << *this << std::endl;
		fout.close();
	}


	Texts::Texts(bool summary, bool each, bool words_stat, bool chars_stat, const std::string outFile) : summary(summary), each(each), words_stat(words_stat), chars_stat(chars_stat), outFile(outFile)
	{

	}
	Texts::Texts(const std::vector<std::string> paths, bool summary, bool each, bool words_stat, bool chars_stat, const std::string outFile) : summary(summary), each(each), words_stat(words_stat), chars_stat(chars_stat), outFile(outFile)
	{
		updatePaths(paths);
	}
	Texts::~Texts()
	{

	}

	void Texts::singleTextAnalys(bulka::VLT& text) {
		text.load();
	}

	void Texts::analys() {
		try {
			timer.start();
			timer.setName("Texts");
			std::cout << "Analyzing many files" << std::endl;
			std::vector<std::thread> threads;
			threads.reserve(texts.size());
			for (auto& text : texts) {
				threads.emplace_back([&text, this]() {singleTextAnalys(text); });
			}
			for (auto& t : threads) {
				if (t.joinable()) {
					t.join();
				}
			}
			for (auto& text : texts) {
				for (auto& word : text.getWords()) {
					words_sum[word.first] += word.second;
				}
				for (int i = 0; i < 256; ++i) {
					letters_sum[i] += text.getLetters()[i];
				}
				words_count_sum += text.getWordsCount();
				lines_count_sum += text.getLinesCount();
				text_lines_count_sum += text.getTextLinesCount();
				length_sum += text.getLength();
			}

			timer.stop();
			std::cout << "Analyzed many files. Time: " << timer.getTimeSeconds() << "s" << std::endl;
		}
		catch (std::exception exception) {
			std::cerr << "Error in texts analys: " << exception.what() << std::endl;
		}
	}
	std::ostream& operator<< (std::ostream& out, VLT& vlt) {
		out << "Analyzed file: " << vlt.path << "\nElapsed time - " << vlt.timer.getTimeSeconds() << "s\n";
		out << "Statistics: \n\tLength: " << vlt.length << "\n\tLines: " << vlt.lines_count << "\n\tText Lines: " << vlt.text_lines_count << "\n\tWords: " << vlt.words_count << "\n\tUnique words: " << vlt.words.size() << "\n\n";

		if(vlt.words_stat){
			std::vector<std::pair<std::string, unsigned int>> vec_words;
			vec_words.reserve(vlt.words.size());
			for (const auto& pair : vlt.words) {
				vec_words.push_back(pair);
			}
			std::sort(vec_words.begin(), vec_words.end(), comparator);
			for (size_t i = 0; i < vec_words.size(); ++i)
			{
				auto& pair = vec_words[i];
				out << i + 1 << ". " << pair.first << ": " << pair.second << "\n";
			}
		}
		if(vlt.chars_stat){
			std::vector<std::pair<char, long long>> letters_vec(256);
			for (int i = 0; i < 256; ++i)
			{
				letters_vec[i] = std::make_pair(i, vlt.letters[i]);
			}
			std::sort(letters_vec.begin(), letters_vec.end(), comparator_char);
			out << "\n\n\n" << "Chars: " << "\n";
			for (size_t i = 0; i < letters_vec.size(); ++i)
			{
				auto& pair = letters_vec[i];
				if (pair.second != 0)
				{
					out << i + 1 << ". " << pair.first << ": " << pair.second << "\n";
				}
			}
		}

		return out;
	}

	std::ostream& operator<< (std::ostream& out, Texts& texts) {
		out << "Analyzed files:\n";
		for (size_t i = 0; i < texts.texts.size(); i++)
		{
			out << "\t" << texts.texts[i].getPath() << "\n";
		}
		out << "Elapsed time - " << texts.timer.getTimeSeconds() << "s\n";
		out << "Statistics: \n\tLength: " << texts.length_sum << "\n\tLines: " << texts.lines_count_sum << "\n\tText Lines: " << texts.text_lines_count_sum << "\n\tWords: " << texts.words_count_sum << "\n\tUnique words: " << texts.words_sum.size() << "\n\n";

		if (texts.words_stat) {
			std::vector<std::pair<std::string, unsigned int>> vec_words;
			vec_words.reserve(texts.words_sum.size());
			for (const auto& pair : texts.words_sum) {
				vec_words.push_back(pair);
			}
			std::sort(vec_words.begin(), vec_words.end(), comparator);
			for (size_t i = 0; i < vec_words.size(); ++i)
			{
				auto& pair = vec_words[i];
				out << i + 1 << ". " << pair.first << ": " << pair.second << "\n";
			}
		}
		if (texts.chars_stat) {
			std::vector<std::pair<char, long long>> letters_vec(256);
			for (int i = 0; i < 256; ++i)
			{
				letters_vec[i] = std::make_pair(i, texts.letters_sum[i]);
			}
			std::sort(letters_vec.begin(), letters_vec.end(), comparator_char);
			out << "\n\n\n" << "Chars: " << "\n";
			for (size_t i = 0; i < letters_vec.size(); ++i)
			{
				auto& pair = letters_vec[i];
				if (pair.second != 0)
				{
					out << i + 1 << ". " << pair.first << ": " << pair.second << "\n";
				}
			}
		}

		return out;
	}
	void Texts::printResultSum() {
		std::cout << *this << std::endl;
	}
	void Texts::printResultAll() {
		printResultSum();
		for (auto& text : texts) {
			text.printResult();
		}
	}
	void Texts::writeResultSumInFile() {
		if (outFile.empty()) {
			return;
		}
		std::ofstream fout;
		fout.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		//fout.imbue(std::locale(locale));
		fout.open(outFile);

		if (!fout.is_open()) {
			throw std::exception("FOUT IS NOT OPEN!");
		}

		if (summary) {
			fout << *this << std::endl;
		}
		fout.close();
	}
	void Texts::writeResultAllInFile() {
		writeResultSumInFile();
		if(each){
			for (auto& text : texts) {
				text.writeResultInFile(true);
			}
		}
	}


	void Texts::updatePaths(std::vector<std::string> paths)
	{
		for (size_t i = 0; i < paths.size(); i++)
		{
			char* str = new char[paths[i].length() + 1];
			str[paths[i].length()] = '\0';
			for (size_t j = 0; j < paths[i].length() + 1; j++)
			{
				str[j] = paths[i][j];
			}
			texts.push_back(VLT(str, words_stat, chars_stat, outFile));
		}
	}

	bool parseArguments(int argc, char* argv[],
		bool& recursively, bool& summary, bool& print, std::string& outFile, std::vector<std::string>& files, bool& each,
		bool& words_stat, bool& chars_stat
	) {
		for (size_t i = 1; i < argc; i++)
		{
			const char* arg = argv[i];
			if (arg[0] == '-') {
				for (int ptr = 1; arg[ptr] != '\0'; ++ptr) {
					if (arg[ptr] == 'r') {
						recursively = true;
					}
					else if (arg[ptr] == 'S') {
						summary = false;
					}
					else if (arg[ptr] == 'E') {
						each = false;
					}
					else if (arg[ptr] == 'W') {
						words_stat = false;
					}
					else if (arg[ptr] == 'A') {
						chars_stat = false;
					}
					else if (arg[ptr] == 'p') {
						print = true;
					}
					else if (arg[ptr] == 'h' || arg[ptr] == '?') {
						return true;
					}
					else if (arg[ptr] == 'o') {
						if (i + 1 == argc || argv[i + 1][0] == '-') {
							throw std::invalid_argument("Invalid arguments!\n\tAfter -o flag (output file) must be a name of file!!!");
						}
						outFile = argv[i + 1];
						++i;
					}
					else if (arg[ptr] == 'c') {
						if (i + 1 == argc || argv[i + 1][0] == '-') {
							throw std::invalid_argument("Invalid arguments!\n\tAfter -c flag (delimiters charset) must be a charset!!!");
						}
						std::string parsed = bcppul::parseEscapeSequences(std::string(argv[i + 1]));
						delimiters = new char[parsed.length() + 1];
						delimiters[parsed.length()] = '\0';
						strncpy(delimiters, parsed.c_str(), parsed.length());
						++i;
					}
					else if (arg[ptr] == 'l') {
						if (i + 1 == argc || argv[i + 1][0] == '-') {
							throw std::invalid_argument("Invalid arguments!\n\tAfter -l flag (locale) must be a locale!!!");
						}
						++i;
						setlocale(LC_ALL, locale);
					}
					else {
						throw std::invalid_argument("Invalid arguments!\n\tUnknown flag \'" + std::string(arg) + "\'!!!");
					}
				}

			}
			else {
				if (std::filesystem::is_regular_file(arg)) {
					files.push_back(arg);
				}
				else if (std::filesystem::is_directory(arg)) {
					std::vector<std::string> dir_files = bcppul::getFilesInDirectory(arg, recursively);
					files.insert(files.end(), dir_files.begin(), dir_files.end());
				}
			}
		}
		if (files.empty()) {
			std::vector<std::string> dir_files = bcppul::getFilesInDirectory(".", recursively);
			files.insert(files.end(), dir_files.begin(), dir_files.end());
		}
		std::string first_arg(argv[0]);
#ifdef _WIN32
		std::string exe_name = first_arg.substr(first_arg.find_last_of("\\") + 1);
#endif
#ifdef __linux__
		std::string exe_name = first_arg.substr(first_arg.find_last_of("/") + 1);
#endif
		files.erase(std::remove(files.begin(), files.end(), "./" + exe_name), files.end());
		if(!outFile.empty()) {
			files.erase(std::remove(files.begin(), files.end(), "./" + outFile), files.end());
		}
		return false;
	}


	void largeTextAnalysis(int argc, char* argv[]) {
		setlocale(LC_ALL, locale);
		bcppul::SimpleTimer timer("root");
		bool recursively = false;
		bool summary = true;
		bool print = false;
		bool each = true;
		bool words_stat = true;
		bool chars_stat = true;
		std::string outFile = "";
		std::vector<std::string> files;
		bool help = parseArguments(argc, argv, recursively, summary, print, outFile, files, each, words_stat, chars_stat);
		std::cout << "Welcome to Bulka`s C++ text analizer!\nSource code: https://github.com/STimofii/LargeTextAnalysis\n";

		if(!help){
			std::cout << std::boolalpha << "Settings:\n\tRecursively scan directory - " << recursively << "\n\tSummary out - " << summary << "\n\tOut of Each file - " << each << "\n\tPrint out in console - " << print << "\n\tWords statistics - " << words_stat << "\n\tChars statistics - " << chars_stat;
			if (!outFile.empty()) {
				std::cout << "\n\tOut in file - " << outFile;
			}
			else {
				std::cout << "\n\tOut in file - false";
			}
			std::cout << "\n\tLocale - " << locale;
			std::cout << "\n\tDelimiters - \'" << bcppul::replaceEscapesWithRaw(delimiters) << "\'";
			std::cout << "\nFiles for analize: " << files.size();
			for (size_t i = 0; i < files.size(); i++) {
				std::cout << "\n\t" << i + 1 << ". " << files[i];
			}

			std::cout << "\n\nStarting!!!\n" << std::noboolalpha << std::endl;
			Texts texts(files, summary, each, words_stat, chars_stat, outFile);
			texts.analys();
			if (print)
				texts.printResultAll();
			texts.writeResultAllInFile();
		}
		else {
			std::cout << "Usage: \n"; 
			std::cout << "LargeTextAnalys[-l ua][-S][-r][-p][-o out.txt][-c \" ., -;\\t\"] <analize directory/directories/files (this directory if you don't specified)>\n";
			std::cout << "\tr - recursively scan directory\n";
			std::cout << "\tS - disable summary\n";
			std::cout << "\tE - disable output of each text\n";
			std::cout << "\tp - print results in console\n";
			std::cout << "\to - save results in file\n";
			std::cout << "\tl - set locale\n";
			std::cout << "\tc - delimiters charset\n";
			std::cout << "\tW - disable words statistics\n";
			std::cout << "\tA - disable chars statistics\n";
		}
	}



	const std::string& VLT::getPath() const {
		return path;
	}
	void VLT::setPath(std::string& path) {
		this->path = path;
	}
	const std::string& VLT::getOutFile() const {
		return outFile;
	}
	void VLT::setOutFile(std::string& outFile) {
		this->outFile = outFile;
	}
	const std::string& Texts::getOutFile() const {
		return outFile;
	}
	void Texts::setOutFile(std::string& outFile) {
		this->outFile = outFile;
		for (auto& text : texts) {
			text.setOutFile(outFile);
		}
	}

	std::unordered_map<std::string, unsigned long long>& VLT::getWords() {
		return words;
	}
	unsigned long long* VLT::getLetters() {
		return letters;
	}
	bcppul::Timer& VLT::getTimer() {
		return timer;
	}


	unsigned long long  VLT::getWordsCount() const {
		return words_count;
	}
	unsigned long long  VLT::getLinesCount() const {
		return lines_count;
	}
	unsigned long long VLT::getTextLinesCount() const {
		return text_lines_count;
	}
	unsigned long long  VLT::getLength() const {
		return length;
	}
	std::unordered_map<std::string, unsigned long long>& Texts::getWordsSum() {
		return words_sum;
	}
	unsigned long long* Texts::getLettersSum() {
		return letters_sum;
	}
	bcppul::Timer& Texts::getTimer() {
		return timer;
	}


	unsigned long long Texts::getWordsCountSum() const {
		return words_count_sum;
	}
	unsigned long long Texts::getLinesCountSum() const {
		return lines_count_sum;
	}
	unsigned long long Texts::getTextLinesCountSum() const
	{
		return text_lines_count_sum;
	}
	unsigned long long Texts::getLengthSum() const {
		return length_sum;
	}
	bool Texts::isSummary() const {
		return summary;
	}
	void Texts::setSummary(bool summary) {
		this->summary = summary;
	}
	bool Texts::isEach() const {
		return each;
	}
	void Texts::setEach(bool each) {
		this->each = each;
	}
	bool Texts::isWordsStat() const {
		return words_stat;
	}
	void Texts::setWordsStat(bool words_stat) {
		this->words_stat = words_stat;
	}
	bool Texts::isCharsStat() const {
		return chars_stat;
	}
	void Texts::setCharsStat(bool chars_stat) {
		this->chars_stat = chars_stat;
	}
	bool VLT::isWordsStat() const {
		return words_stat;
	}
	void VLT::setWordsStat(bool words_stat) {
		this->words_stat = words_stat;
	}
	bool VLT::isCharsStat() const {
		return chars_stat;
	}
	void VLT::setCharsStat(bool chars_stat) {
		this->chars_stat = chars_stat;
	}
} //namespace bulka





int main(int argc, char* argv[])
{
	try
	{
		bulka::largeTextAnalysis(argc, argv);
		delete[] bulka::delimiters;
		bulka::delimiters = nullptr;
	}
	catch (std::exception exception) {
		std::cerr << "Error in main: " << exception.what() << std::endl;
	}

	return 0;
}