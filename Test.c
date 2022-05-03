#include <stdio.h>
#include "json.hpp"
#include <lomanip>

using json = nlohmann::json;


int main() {
   printf("Test de AWS");
   char text[] =R"(
   {
      "request": "POST_CHAT",
      "body" :"all"
   })";
   json j_complete = json::parse(text);
   std::cout <<std::setw(4) << j_complete <<std::endl;


   return 0;
}