// LargeTextAnalysis.cpp

#include "LargeTextAnalysis.h"

const char* delimiters = " .,:-1234567890(){};\"\'`[]/—«»!?„“…№#$@*\n\t\r";

bool comparator (const std::pair<std::string, unsigned int>& a, const std::pair<std::string, unsigned int>& b) {
	if (a.second != b.second) {
		return a.second > b.second;
	}
	return a.first > b.first;
}
bool comparator_char (const std::pair<char, long long>& a, const std::pair<char, long long>& b) {
	if (a.second != b.second) {
		return a.second > b.second;
	}
	return a.first > b.first;
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
		std::ifstream fin;
		//fin.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fin.imbue(std::locale("ru"));
		fin.open(path);

		if(!fin.is_open()){
			throw std::exception("FIN IS NOT OPEN!");
		}
		std::string line;
		while (std::getline(fin, line)) {
			++lines_count;
			for (size_t i = 0; i < line.length(); ++i)
			{
				++length;
				++letters[static_cast<unsigned char>(line[i])];
			}
			std::transform(line.begin(), line.end(), line.begin(),
				[](unsigned char c) { return std::tolower(c); });
			size_t left = 0;
			for (size_t i = 0; i < line.length(); ++i)
			{
				char c = line[i];
				for (size_t j = 0; delimiters[j] != '\0'; ++j)
				{
					if (c == delimiters[j]) {
						size_t also = 1;
						for (; i + also < line.length(); ++also)
						{
							bool flag = false;
							for (size_t j = 0; delimiters[j] != '\0'; ++j)
							{
								if (line[i + also] == delimiters[j]) {
									flag = true;
								}
							}
							if(!flag)
								break;
						}
						std::string word = line.substr(left, i - left);
						i += also;
						left = i;
						words[word] += 1;
						++words_count;
					}
				}
			}
		}
		

		fin.close();
	}

	void analys() {
		try {
			load();
		}
		catch (std::exception& exception) {
			std::cerr << "Error in text analys: " << exception.what() << std::endl;
		}
		print();

		
	}
	void print() {
		for (const auto& pair : words) {
			vec_words.push_back(pair);
		}
		std::sort(vec_words.begin(), vec_words.end(), comparator);

		std::vector<std::pair<char, long long>> letters_vec(256);
		for (int i = 0; i < 256; ++i)
		{
			letters_vec[i] = std::make_pair(i, letters[i]);
		}
		std::sort(letters_vec.begin(), letters_vec.end(), comparator_char);

		std::cout << "Length: " << length << "\tLines: " << lines_count << "\tWords: " << words_count << "\tUnique words: " << vec_words.size() << "\n\n";
		
		for (size_t i = 0; i < vec_words.size(); ++i)
		{
			auto& pair = vec_words[i];
			std::cout << i + 1 << ". " << pair.first << ": " << pair.second << "\n";
		}
		std::cout << "\n\n\n" << "Chars: " << "\n";
		for (size_t i = 0; i < letters_vec.size(); ++i)
		{
			auto& pair = letters_vec[i];
			if(pair.second != 0)
			{
				std::cout << i + 1 << ". " << pair.first << ": " << pair.second << "\n";
			}
		}
	}
};


int main()
{
	setlocale(LC_ALL, "ru");
	bcppul::SimpleTimer timer;
	VLT vlt("../../../../texts/text.txt");
	vlt.analys();
	//TODO: Many files; Console args


	return 0;
}
