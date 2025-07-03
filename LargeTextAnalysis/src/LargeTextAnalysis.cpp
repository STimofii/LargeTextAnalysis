// LargeTextAnalysis.cpp

#include "LargeTextAnalysis.h"

namespace bulka {
	const char* delimiters = " .,:–—1234567890(){};\"\'`[]/«»!?„“…№#$@*\n\t\r";

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


	class VLT {
	private:
		const char* path = nullptr;
		std::unordered_map<std::string, unsigned int> words;
		std::vector<std::pair<std::string, unsigned int>> vec_words;
		long long letters[256]{};
		long long words_count = 0;
		long long lines_count = 0;
		long long length = 0;
		bcppul::Timer timer;
	protected:
	public:
		VLT()
		{

		}
		VLT(const char* path) : path(path)
		{

		}
		~VLT() {
		}


		void load() {
			std::cout << "Analyzing " << path << std::endl;
			timer.start();
			std::ifstream fin;
			//fin.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			//fin.imbue(std::locale("ru"));
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

		void analys() {
			try {
				load();
			}
			catch (std::exception& exception) {
				std::cerr << "Error in text analys: " << exception.what() << std::endl;
			}


		}

		friend std::ostream& operator<< (std::ostream& out, VLT& vlt) {
			vlt.vec_words.reserve(vlt.words.size());
			for (const auto& pair : vlt.words) {
				vlt.vec_words.push_back(pair);
			}
			std::sort(vlt.vec_words.begin(), vlt.vec_words.end(), comparator);

			std::vector<std::pair<char, long long>> letters_vec(256);
			for (int i = 0; i < 256; ++i)
			{
				letters_vec[i] = std::make_pair(i, vlt.letters[i]);
			}
			std::sort(letters_vec.begin(), letters_vec.end(), comparator_char);

			out << "Analyzed file: " << vlt.path << "\nElapsed time - " << vlt.timer.getTimeSeconds() << "s\n";
			out << "Length: " << vlt.length << "\tLines: " << vlt.lines_count << "\tWords: " << vlt.words_count << "\tUnique words: " << vlt.vec_words.size() << "\n\n";

			for (size_t i = 0; i < vlt.vec_words.size(); ++i)
			{
				auto& pair = vlt.vec_words[i];
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
		void printResult() {
			std::cout << *this << std::endl;
		}
		void writeResultInFile(const char* outFilePath) {
			writeResultInFile(outFilePath, false);
		}
		void writeResultInFile(const char* outFilePath, bool append) {
			std::ofstream fout;
			fout.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fout.imbue(std::locale("ru"));
			if(append)
			{
				fout.open(outFilePath, std::fstream::ios_base::app);
			}
			else
			{
				fout.open(outFilePath);
			}

			if (!fout.is_open()) {
				throw std::exception("FOUT IS NOT OPEN!");
			}

			fout.imbue(std::locale("C"));
			fout << *this << std::endl;
			fout.close();
		}


		const char* getPath() const {
			return path;
		}
		void setPath(const char* path) {
			this->path = path;
		}
		std::unordered_map<std::string, unsigned int>& getWords() {
			return words;
		}
		std::vector<std::pair<std::string, unsigned int>>& getVectorWords() {
			return vec_words;
		}
		long long* getLetters() {
			return letters;
		}
		bcppul::Timer& getTimer() {
			return timer;
		}


		long long getWordsCount() {
			return words_count;
		}
		long long getLinesCount() {
			return lines_count;
		}
		long long getLength() {
			return length;
		}

	};

	class Texts {
	private:
		std::vector <bulka::VLT> texts;
		std::unordered_map<std::string, unsigned int> words_sum;
		std::vector<std::pair<std::string, unsigned int>> vec_words_sum;
		long long letters_sum[256]{};
		long long words_count_sum = 0;
		long long lines_count_sum = 0;
		long long length_sum = 0;
		bcppul::Timer timer;
	protected:
	public:
		Texts()
		{

		}
		Texts(std::vector<const char*> paths)
		{
			updatePaths(paths);
		}
		~Texts()
		{

		}

		void singleTextAnalys(bulka::VLT& text) {
			text.load();
		}

		void analys() {
			try {
				timer.start();
				std::cout << "Analyzing many files" << std::endl;
				std::vector<std::thread> threads;
				threads.reserve(texts.size());
				for (auto& text : texts) {
					threads.emplace_back([&text, this](){singleTextAnalys(text);});
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

		friend std::ostream& operator<< (std::ostream& out, Texts& texts) {
			texts.vec_words_sum.reserve(texts.words_sum.size());
			for (const auto& pair : texts.words_sum) {
				texts.vec_words_sum.push_back(pair);
			}
			std::sort(texts.vec_words_sum.begin(), texts.vec_words_sum.end(), comparator);

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
			out << "Length: " << texts.length_sum << "\tLines: " << texts.lines_count_sum << "\tWords: " << texts.words_count_sum << "\tUnique words: " << texts.vec_words_sum.size() << "\n\n";

			for (size_t i = 0; i < texts.vec_words_sum.size(); ++i)
			{
				auto& pair = texts.vec_words_sum[i];
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
		void printResultSum() {
			std::cout << *this << std::endl;
		}
		void printResultAll() {
			printResultSum();
			for (auto& text : texts) {
				text.printResult();
			}
		}
		void writeResultSumInFile(const char* outFilePath) {
			std::ofstream fout;
			fout.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fout.imbue(std::locale("ru"));
			fout.open(outFilePath);

			if (!fout.is_open()) {
				throw std::exception("FOUT IS NOT OPEN!");
			}

			fout.imbue(std::locale("C"));
			fout << *this << std::endl;
			fout.close();
		}
		void writeResultAllInFile(const char* outFilePath) {
			writeResultSumInFile(outFilePath);
			for (auto& text : texts) {
				text.writeResultInFile(outFilePath, true);
			}
		}
		
		
		void updatePaths(std::vector<const char*> paths)
		{
			for (size_t i = 0; i < paths.size(); i++)
			{
				texts.push_back(VLT(paths[i]));
			}
		}

		std::unordered_map<std::string, unsigned int>& getWordsSum() {
			return words_sum;
		}
		std::vector<std::pair<std::string, unsigned int>>& getVectorWordsSum() {
			return vec_words_sum;
		}
		long long* getLettersSum() {
			return letters_sum;
		}
		bcppul::Timer& getTimer() {
			return timer;
		}


		long long getWordsCountSum() {
			return words_count_sum;
		}
		long long getLinesCountSum() {
			return lines_count_sum;
		}
		long long getLengthSum() {
			return length_sum;
		}
	};

} //namespace bulka

int main()
{
	setlocale(LC_ALL, "ru");
	bcppul::SimpleTimer timer;
	std::vector<const char*> paths;
	paths.push_back("../../../../texts/metro2033.txt");
	paths.push_back("../../../../texts/metro2034.txt");
	paths.push_back("../../../../texts/metro2035.txt");
	bulka::Texts texts(paths);
	texts.analys();
	//texts.printResultAll();
	texts.writeResultAllInFile("out.txt");
	//TODO: Console args


	return 0;
}