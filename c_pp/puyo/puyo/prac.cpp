#include <iostream>

template <class T>
T power( T x, int y ){
	T result =1;
	if (x < 0){
		return (0);
	}else{
		for (int i=0;i < y;i++){
			result *= x;
		}
	}
	return  result;
}

int main(){
	std::cout << "power(2,16) = " << power(2,16) << std::endl;
	std::cout << "power(1.5,4) = " << power(1.5,6) << std::endl;
}

