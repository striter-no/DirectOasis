#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>

namespace utils::io{
    
    std::string input(std::string message);
    void input();
    
	class Print{
	public:
		template<class T>
		static void print(T el, char end = '\n', char start = '\r'){
			std::cout<< start <<el<<end;
		}

		template<class T>
		static void print(std::vector<T> vec, char end = '\n', char start = '\r'){
			std::cout << start <<'[';
			for(int i = 0; i < vec.size(); i++){
				std::cout<<'"';
				print(vec[i], ' ', ' ');
				if(i != vec.size()-1){
					std::cout<<"\", ";
				} else {
					std::cout<<'"';
				}
			}
			std::cout<<']'<<end;
		}

		template<class K, class V>
		static void print(std::pair<K, V> el, char end = '\n', char start = '\r'){
			std::cout << start << '{';
			print(el.first, ' ', ' '); std::cout << ':';
			print(el.second, ' ', ' '); std::cout << '}';
		}

		template<class K, class V>
		static void print(std::map<K, V> obj, char end = '\n', char start = '\r'){
			int i = 0;
			for(auto &pair: obj){
				std::cout << start << '{';
				print(pair.first, ' ', ' '); std::cout << ':';
				print(pair.second, ' ', ' '); std::cout << '}';
				if(i < obj.size() - 1)
					std::cout << ',' << std::endl;
				i++;
			}
			std::cout << end;
		}

		template<class K, class V>
		static void print(std::unordered_map<K, V> obj, char end = '\n', char start = '\r'){
			int i = 0;
			for(auto &pair: obj){
				std::cout << start << '{';
				print(pair.first, ' ', ' '); std::cout << ':';
				print(pair.second, ' ', ' '); std::cout << '}';
				if(i < obj.size() - 1)
					std::cout << ',' << std::endl;
				i++;
			}
			std::cout << end;
		}
	};

	std::string input(std::string message){
		std::cout<<message;
		std::getline(std::cin, message);
		return message;
	}

	void input(){
		std::cout<<"> ";
		char buf;
		std::cin>>buf;
	}

}