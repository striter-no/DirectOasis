#pragma once
#include <fstream>
#include <string>

namespace utils::fls{
    // Функция получения содержимого файла как строки
	std::string getFile(std::string filename);
	// Функция записи информации в файл
	void writeFile(std::string filename, std::string content);

    std::string getFile(std::string filename){
		std::ifstream ifs(filename);
		if(!ifs.is_open())
			throw std::invalid_argument("Cannot open this file for reading: "+filename+" reason: unreachable");
		std::string file((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		ifs.close();

		return {file};
	}

	void writeFile(std::string filename, std::string content){
		std::ofstream ofs(filename);
        if(!ofs.is_open())
            throw std::invalid_argument("Cannot open this file for writing: "+filename+" reason: unreachable");
        ofs << content;
        ofs.close();
	}
}