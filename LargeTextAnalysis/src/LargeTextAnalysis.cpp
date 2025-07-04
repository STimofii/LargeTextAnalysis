// LargeTextAnalysis.cpp

#include "LargeTextAnalysis.h"

const char* locale = "ua";

namespace bulka {
	bool comparator(const std::pair<std::string, unsigned int>& a, const std::pair<std::string, unsigned int>& b) {
		if (a.second != b.second) {
			return a.second > b.second;
		}
		return a.first < b.first;
	}
	bool comparator_char(const std::pair<char, long long>& a, const std::pair<char, long long>& b) {
		if (a.second != b.second) {
			return a.second > b.second;
		}
		return a.first < b.first;
	}


	VLT::VLT(const std::string path, const std::string outFile) : path(path), outFile(outFile)
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
			if (line.size() == 0 || line.empty()) {
				continue;
			}
			++lines_count;
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


	Texts::Texts(bool summary, bool each, const std::string outFile) : summary(summary), each(each), outFile(outFile)
	{

	}
	Texts::Texts(const std::vector<std::string> paths, bool summary, bool each, const std::string outFile) : summary(summary), each(each), outFile(outFile)
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
		std::vector<std::pair<std::string, unsigned int>> vec_words;
		vec_words.reserve(vlt.words.size());
		for (const auto& pair : vlt.words) {
			vec_words.push_back(pair);
		}
		std::sort(vec_words.begin(), vec_words.end(), comparator);

		std::vector<std::pair<char, long long>> letters_vec(256);
		for (int i = 0; i < 256; ++i)
		{
			letters_vec[i] = std::make_pair(i, vlt.letters[i]);
		}
		std::sort(letters_vec.begin(), letters_vec.end(), comparator_char);

		out << "Analyzed file: " << vlt.path << "\nElapsed time - " << vlt.timer.getTimeSeconds() << "s\n";
		out << "Length: " << vlt.length << "\tLines: " << vlt.lines_count << "\tWords: " << vlt.words_count << "\tUnique words: " << vec_words.size() << "\n\n";

		for (size_t i = 0; i < vec_words.size(); ++i)
		{
			auto& pair = vec_words[i];
			out << i + 1 << ". " << pair.first << ": " << pair.second << "\n";
		}
		out << "\n\n\n" << "Chars: " << "\n";
		for (size_t i = 0; i < letters_vec.size(); ++i)
		{
			auto& pair = letters_vec[i];
			if (pair.second != 0)
			{
				out << i + 1 << ". " << pair.first << ": " << pair.second << "\n";
			}
		}

		return out;
	}

	std::ostream& operator<< (std::ostream& out, Texts& texts) {
		std::vector<std::pair<std::string, unsigned int>> vec_words_sum;
		vec_words_sum.reserve(texts.words_sum.size());
		for (const auto& pair : texts.words_sum) {
			vec_words_sum.push_back(pair);
		}
		std::sort(vec_words_sum.begin(), vec_words_sum.end(), comparator);

		std::vector<std::pair<char, long long>> letters_vec_sum(256);
		for (int i = 0; i < 256; ++i)
		{
			letters_vec_sum[i] = std::make_pair(i, texts.letters_sum[i]);
		}
		std::sort(letters_vec_sum.begin(), letters_vec_sum.end(), comparator_char);

		out << "Analyzed files:\n";
		for (size_t i = 0; i < texts.texts.size(); i++)
		{
			out << "\t" << texts.texts[i].getPath() << "\n";
		}
		out << "Elapsed time - " << texts.timer.getTimeSeconds() << "s\n";
		out << "Length: " << texts.length_sum << "\tLines: " << texts.lines_count_sum << "\tWords: " << texts.words_count_sum << "\tUnique words: " << vec_words_sum.size() << "\n\n";

		for (size_t i = 0; i < vec_words_sum.size(); ++i)
		{
			auto& pair = vec_words_sum[i];
			out << i + 1 << ". " << pair.first << ": " << pair.second << "\n";
		}
		out << "\n\n\n" << "Chars: " << "\n";
		for (size_t i = 0; i < letters_vec_sum.size(); ++i)
		{
			auto& pair = letters_vec_sum[i];
			if (pair.second != 0)
			{
				out << i + 1 << ". " << pair.first << ": " << pair.second << "\n";
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


	void Texts::updatePaths(std::vector<const char*> paths)
	{
		for (size_t i = 0; i < paths.size(); i++)
		{
			texts.push_back(VLT(paths[i], outFile));
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
			texts.push_back(VLT(str, outFile));
		}
	}

	void parseArguments(int argc, char* argv[],
		bool& recursively, bool& summary, bool& print, std::string& outFile, std::vector<std::string>& files, bool& each
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
					else if (arg[ptr] == 'p') {
						print = true;
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
	}


	void largeTextAnalysis(int argc, char* argv[]) {
		setlocale(LC_ALL, locale);
		bcppul::SimpleTimer timer("root");
		bool recursively = false;
		bool summary = true;
		bool print = false;
		bool each = true;
		std::string outFile = "";
		std::vector<std::string> files;
		parseArguments(argc, argv, recursively, summary, print, outFile, files, each);
		std::cout << std::boolalpha << "Welcome to Bulka`s C++ text analizer!\nSettings:\n\tRecursively scan directory- " << recursively << "\n\tSummary out - " << summary << "\n\tOut of Each file - " << each << "\n\tPrint out in console - " << print;
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
			std::cout << "\n\t" << i+1 << ". " << files[i];
		}
		
		std::cout << "\n\nStarting!!!\n" << std::noboolalpha << std::endl;
		Texts texts(files, summary, each, outFile);
		texts.analys();
		if (print)
			texts.printResultAll();
		texts.writeResultAllInFile();
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

	std::unordered_map<std::string, unsigned int>& VLT::getWords() {
		return words;
	}
	long long* VLT::getLetters() {
		return letters;
	}
	bcppul::Timer& VLT::getTimer() {
		return timer;
	}


	long long  VLT::getWordsCount() {
		return words_count;
	}
	long long  VLT::getLinesCount() {
		return lines_count;
	}
	long long  VLT::getLength() {
		return length;
	}
	std::unordered_map<std::string, unsigned int>& Texts::getWordsSum() {
		return words_sum;
	}
	long long* Texts::getLettersSum() {
		return letters_sum;
	}
	bcppul::Timer& Texts::getTimer() {
		return timer;
	}


	long long Texts::getWordsCountSum() {
		return words_count_sum;
	}
	long long Texts::getLinesCountSum() {
		return lines_count_sum;
	}
	long long Texts::getLengthSum() {
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