#include <iostream>
#include <string>
int main(void)
{

  std::string str1("abc");
  std::string str2("def");
  
  str1 = str1 + str2 + "zzz";

  std::cout << str1 << std::endl;

  return 0;

}
