#pragma once
#include <vector>
#include <iostream>
#include <map>
#include <functional>
#include "string.hpp"

namespace utils::vec{
    std::vector<std::string> stripsplit(std::string line, char delim = ' ', char strp = ' ');
    std::string join(std::vector<std::string> vs, char udel = ' ');
    std::string remove(std::string str, std::vector<char> vchs);
    std::vector<int> range(int rng, int b = -1);
    std::vector<std::string> split(std::string line, char delim = ' ');
    std::string join(std::vector<std::string> vs, std::string udel);

    template<class vc>
	int index(const std::vector<vc> &vecs, vc key, int start_index = 0){
		for(int i = start_index; i < vecs.size(); i++){
			if(vecs[i] == key) return i;
		}
		return -1;
	}

    template<class kc>
	bool in(std::vector<kc> v, kc i){
		for(kc k : v){
			if(k == i){
				return true;
			}
		}
		return false;
	}

    template<class TString> 
    std::vector<TString> outEmptys(std::vector<TString> vc){
		std::vector<TString> ovc;
		for(TString&str:vc){
			if(!str.empty()){
				ovc.push_back(str);
			}
		}
		return {ovc};
	}

	template<class T>
	std::vector<T> sliceVec(std::vector<T> vc, int inda, int indb){
		return std::vector<T>(vc.begin() + inda, 
						(indb <= 0) ? 
							(vc.end() + indb) : 
							(indb < vc.size() ? 
								(vc.begin() + indb) : 
								(vc.end())
							)
						);
    }
    
    template<class T>
	int count(const std::vector<T> &vc, const T el){
		int out = 0; for(const T&el_:vc){
			if(el==el_) out++;
		}
		return {out};
	}

    template<class T>
	std::vector<T> getVectorWithVals(int vsize, T val){
		std::vector<T> o;
		for(T i:range(vsize)){
			o.push_back(val);
		}
		return {o};
	}

	template<class T>
	std::vector<T> vcopy(std::vector<T> &x){
		std::vector<T> to;
		for(T el: x){
			to.push_back(el);
		}
		return {to};
	}

	template<class T>
	void vcopyto(std::vector<T> &to, std::vector<T> from){
		to.clear();
		for(T el: from){
			to.push_back(el);
		}
	}

	template<class T>
	void mergeshift(std::vector<T> &to, const std::vector<T> &from, int shift){
		to.insert(to.begin() + shift, from.begin(), from.end());
	}

	template<class T>
    std::vector<T> merge(std::vector<T> to, const std::vector<T> &from){
        to.insert(to.end(), from.begin(), from.end());
		return to;
	}

	template<class T, class R>
	std::vector<R> processVector(std::vector<T> vec, std::function<R(int, T)> procr){
		std::vector<R> out; int inx = 0; for(T&el:vec){
			out.push_back(procr(inx, el));
			inx++;
		}
		return out; 
	}

	template<class T, class V, class R>
	std::map<R, V> processVectorIntoMap(std::vector<T> vec, std::function<std::pair<R, V>(int, T)> procr){
		std::map<R, V> out; int inx = 0; for(T&el:vec){
			auto res = procr(inx, el);
			out[res.first] = res.second;
			inx++;
		}
		return out; 
	}

	template<class T>
	bool in(std::vector<T> vec, std::function<bool(T&)> comp){
		for(T &el:vec){
			if(comp(el)) return true;
		}
		return false;
	}

	template<class T>
	std::vector<T> selection(std::vector<T> vec, T ideal){
		std::vector<T> out;
		for(T el:vec){
			if(el==ideal) out.push_back(el);
		}
		return out;
	}

	template<class T>
	std::vector<T> selection(std::vector<T> vec, std::function<bool(T&)> comp){
		std::vector<T> out;
		for(T el:vec){
			if(comp(el)) out.push_back(el);
		}
		return out;
	}

	std::vector<int> range(int rng, int b){
		std::vector<int> o;
		if(b==rng){return {};}
		if(b == -1){
			o.resize(rng);
			for(int i = 0; i < rng; i++){
				o[i] = i;
			}
		} else if( b >= 0 && b < rng){
			o.resize(rng);
			for(int i = rng, j = 0; i >= b; i--, j++){
				o[j] = i;
			}
		} else if(b > rng){
			o.resize(b);
			for(int i = rng; i < rng + b; i++){
				o[i-rng] = i;
			}
		}
		return {o};
	}
    
    std::vector<std::string> split(std::string line, char delim){
		if(line.find(delim) != std::string::npos){
			std::vector<std::string> elems{""};
			bool isSpace = true;
			
			for(char ch : line){
				if(isSpace && ch!=delim || !isSpace && ch!=delim){
					elems[elems.size()-1]+=ch;
				}
				isSpace = ch == delim;
				if (isSpace) {
					// if(elems.size() != 0 && elems.back().back() != '\0'){
					// 	elems.back() += '\0';
					// }
					elems.push_back("");
				}
			}

			return outEmptys(elems);
		}

		return {line};
	}

    std::vector<std::string> stripsplit(std::string line, char delim, char strp){
		if(line.find(delim) != std::string::npos){
			std::vector<std::string> elems{""};
			bool isSpace = true;
			
			for(char ch : line){
				if(isSpace && ch!=delim || !isSpace && ch!=delim){
					elems[elems.size()-1]+=ch;
				}
				isSpace = ch == delim;
				if (isSpace) {
					elems.push_back("");
				}
			}

			for(auto &el: elems){
				el = str::strip(el, strp);
			}

			return outEmptys(elems);
		}

		return {line};
	}

    std::string join(std::vector<std::string> vs, char udel){
		std::string o;
		int inx=0;
		for(std::string s : vs){
			o+=s;
			if(inx!=vs.size()-1) o+=udel;
			inx++;
		}

		return {o};
	}

	std::string join(std::vector<std::string> vs, std::string udel){
		std::string o;
		int inx=0;
		for(std::string s : vs){
			o+=s;
			if(inx!=vs.size()-1) o+=udel;
			inx++;
		}

		return {o};
	}
}

namespace utils::str {
	std::string remove(std::string str, std::vector<char> vchs){
		std::string out;
		for(char ch : str){
			if(!vec::in(vchs, ch)){
				out+=ch;
			}
		}
		return {out};
	}

	std::string to_string(std::vector<std::string> vec){
		std::string out = "{";
        int inx=0;
        for(std::string s : vec){
            out+= '"' + s + '"';
            if(inx!=vec.size()-1) out+=", ";
            inx++;
        }
		out+="}";
        return {out};
	}
}