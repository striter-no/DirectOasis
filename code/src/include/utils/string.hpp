#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

namespace utils::str{
    std::string strip(const std::string& str, const char delim = ' ');
    
    int index(std::string str, char dst, int start = 0);
    /*
	aInd - first indx;
	bInd - second indx;
		bInd not changed? - return all str before aInd
		bInd <= 0 - return str from aInd, to strsize + bInd (bInd = 0 => return from aInd to end)
		bInd > 0 - return str from aInd, bInd simbols
	*/
	template<class TString> 
	TString slice(TString str, int aInd = 0, int bInd = 0);
    std::string to_string(char ch);
    bool isInteger(std::string s);
    bool isFloat(std::string s);
    int count(const std::string &vc, const char el);
	bool startswith(const std::string &string, const std::string &stringToFind);

	std::string strip(const std::string& str, const char delim) {
		size_t first = str.find_first_not_of(delim);
		if (std::string::npos == first) {
			return "";
		}
		size_t last = str.find_last_not_of(delim);
		return str.substr(first, (last - first + 1));
	}

    int index(std::string str, char dst, int start){
		for(int i = start; i < str.size(); i++){if(str[i]==dst) {return i;}}
		return -1;
	}
    template<class TString> 
	TString slice(TString str, int aInd, int bInd){
		return TString(str.begin() + aInd, 
						(bInd <= 0) ? 
							(str.end() + bInd) : 
							(bInd <= str.size() ? 
								(str.begin() + bInd) : 
								(str.end())
							)
						);
    }

    std::string to_string(char ch){
		std::string o;o+=ch;
		return {o};
	}

    bool isInteger(std::string s){
		if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;
		char * p;
		strtol(s.c_str(), &p, 10);
		return (*p == 0);
	}

	bool isFloat(std::string s){
		std::istringstream iss(s);
		float f;
		iss >> std::noskipws >> f;
		return iss.eof() && !iss.fail();
	}

    int count(const std::string &vc, const char el){
		int out = 0; for(const char&el_:vc){
			if(el==el_) out++;
		}
		return {out};
	}

	int count(const std::string &vc, std::string el){
		if (el.empty()) return 0;
		if (el.size() > vc.size()) return 0;
		if (el == vc) return 1;

		int out = 0;
		for (int i = 0; i < vc.size() - el.size() + 1; i++) {
			if (el == "denied") std::cout << vc.substr(i, el.size()) << std::endl;
			if (vc.substr(i, el.size()) == el){
				out++;
			}
		}

		return out;
	}

	bool startswith(const std::string &string, const std::string &stringToFind){
		if(stringToFind.size() > string.size()) return false;
		
		for(size_t i = 0; i < stringToFind.size(); ++i){
            if(string[i]!= stringToFind[i]) return false;
        }

		return true;
	}
}