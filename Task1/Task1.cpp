#include <iostream>
#include <list>
#include <string>
#include <set>
#include <io.h>
#include <fstream>
using namespace std;
typedef std::list<std::string> flist;
typedef std::set<std::string>  fset;

void  tokenize(const char* s, fset* exts);
char* copy_ext(char* d, int n, const char* s);
bool  is_ctrl_dir(const char* s);


//поиск файлов с указанными расширениями включая под-папки
bool get_files(const char* folder, const fset* exts, flist* lst) {
	char        ext[1024];
	std::string ps;
	_finddata_t inf;

	bool slash = true;
	ps = folder;
	if ((ps.length() > 0) && (ps[ps.length() - 1] != '\\')) {
		slash = false;
		ps += '\\';
	}
	ps.append("*.*");

	long fp = _findfirst(ps.c_str(), &inf);
	if (fp == -1)
		return false;

	do {

		if (inf.attrib & _A_SUBDIR) { //искать в под-папках

			if (is_ctrl_dir(inf.name)) {
				ps = folder;
				if (!slash)
					ps += '\\';
				ps.append(inf.name);
				get_files(ps.c_str(), exts, lst);
			}
		}
		else {

			copy_ext(ext, sizeof(ext) - 1, inf.name);

			if (exts->find(ext) != exts->end()) {
				lst->push_back(folder);
				if (!slash)
					lst->back() += '\\';
				lst->back().append(inf.name);
			}
		}

	} while (_findnext(fp, &inf) != -1);

	_findclose(fp);
	return true;
}



int main(void) {
	//пример заданий расширений через точку запятой
	char ebuf[64] = "txt;pdf;doc;png;";

	//ввод расширений
	std::cout << "Enter exts: ";
	std::cin.getline(ebuf, sizeof(ebuf) - 1);

	fset exts;
	tokenize(ebuf, &exts);

	//ввод пути 
	char path[255];
	std::cout << "Enter path: ";
	std::cin.getline(path, sizeof(path) - 1);
	std::cin.sync();

	flist fs;
	get_files(path, &exts, &fs);

	//всего найденого файлов
	std::cout << "\tcount find files: "
		<< fs.size() << std::endl;

	//вывести список найденных файлов
	
	//запись списка в файл

	std::ofstream f;
	f.open("text.txt", ios::trunc);
	flist::const_iterator i = fs.begin();
	while (i != fs.end()) {
		f << *i << endl;
		++i;
	}
	f.close();

	fs.clear();
	exts.clear();

	exit(0);
	return 0;
	//std::cin.get();
	//_Exit(1);

	
}

//отделение расширений, пример: "txt;pdf;doc;png;"
void tokenize(const char* s, fset* exts) {
	const char* p;
	while (*s) {
		p = s;
		while (*p && (*p != ';'))
			++p;

		if ((p - s) > 0)
			exts->insert(std::string(s, p));

		if (*p == ';')
			++p;
		s = p;
	}
}

//копируем расширение у имени файла
char* copy_ext(char* d, int n, const char* s) {
	char* t = d;
	while (*s)
		++s;

	s -= 1;
	while ((s != t) && (*s != '.'))
		--s;

	if (*s == '.') {
		++s;
		while (*s)
			* d++ = *s++;
	}
	*d = '\0';
	return t;
}

bool is_ctrl_dir(const char* s) {
	int i = 0, j = 0;
	while (*s) {
		if (*s == '.')
			++i;
		++s;
		++j;
	}
	return (i != j);
}