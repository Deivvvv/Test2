// Test2.cpp: определяет точку входа для приложения.
//

#include "Test2.h"

using namespace std;
struct DualInt {
public:
	int Index{};
	int Num{};
	DualInt() {}
	DualInt(int a, int b) {
		Index = a;
		Num = b;
	}
	~DualInt() {}
};
map<string, DualInt> words;
mutex isLook;
string GetExeFileName()
{
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	return std::string(buffer);
}

string GetExePath()
{
	std::string f = GetExeFileName();
	return f.substr(0, f.find_last_of("\\/"));
}

std::vector<std::string> Split(std::string str, const std::string& token)
{
	std::vector<std::string>result;
	while (str.size())
	{
		int index = str.find(token);
		if (index != std::string::npos)
		{
			result.push_back(str.substr(0, index));
			str = str.substr(index + token.size());
			if (str.size() == 0)
				result.push_back(str);
		}
		else
		{
			result.push_back(str);
			str = "";
		}
	}
	return result;
}

vector<string> ReadFile(string str) {
	vector<string> strings;
	string filePath = GetExePath();
	for (int i = 0; i < 4; i++)
		filePath = filePath.substr(0, filePath.find_last_of("\\/"));
	filePath += "\\Test2\\" + str;
	cout << filePath << endl;
	ifstream file;
	file.open(filePath);
	if (file.is_open()) {

		while (!file.eof()) {
			getline(file, str);
			if (file)
			{
				strings.push_back(str);
				cout << str << endl;
			}
		}

	}
	else
		cout << "File no find." << endl;
	file.close();

	return strings;
}

void ScanFile(string str1, string str2, int num) {

	for (int i = 0; i < str1.size(); i++)
		if (str2.size() + i >= str1.size())
			break;
		else {
			bool use = true;
			for (int j = 0; j < str2.size() && use; j++)
				use = (str1[j + i] == str2[j] || str2[j] == '?');

			if (use) {
				string str = "";
				for (int j = 0; j < str2.size(); j++)
					str.push_back(str1[j + i]);


				DualInt dual(num, i + 1);
				isLook.lock();
				auto search = words.find(str);
				if (search == words.end())
					words[str] = dual;
				isLook.unlock();
				i = str1.size();
			}
		}

}


int main()
{
	string str1 = "input.txt", str2;
	bool use = false;
	while (!use) {
		use = false;
		getline(cin, str2);
		if (str2.size() < 1000) {
			{
				vector<string> strings = Split(str2, "\\n");
				if (strings.size() > 1) {
					cout << "The hyphenation sign \\n is used" << endl;
					str2 = "";
				}

			}
			for (int i = 0; i < str2.size(); i++)
				if (str2[i] == '?')
					if (!use) {
						use = true;
						break;
					}
		}


		if (!use)
			cout << "Invalid request." << endl;
	}

	int threadSize = thread::hardware_concurrency();
	int num = 0;
	vector<string> strings = ReadFile(str1);
	while (num < strings.size())
	{
		vector<thread> threads;
		for (int i = 0; i < threadSize && num < strings.size(); i++) {
			threads.emplace_back(&ScanFile, strings[num], str2, num);
			num++;
		}

		for (auto& thread : threads)
		{
			if (thread.joinable())
				thread.join();
		}
	}

	cout << words.size() << endl;
	for (auto iter = words.begin(); iter != words.end(); ++iter)
	{
		cout << iter->second.Index << " " << iter->second.Num << " " << iter->first << " " << strings[iter->second.Index] << endl;
	}

	return 0;
}
