#include <string>
#include <iostream>
#include <fstream>

using namespace std;

/* The function to update Resources' file, specific for see
 * to refer correctly to the base file
*/
void UpdateSeeXResources (char * filename, char * base) {

std::string res, cur;
size_t pos;
ifstream in(filename);
while (!in.eof()) {
 getline(in, cur);
 if (cur.find("#include") != std::string::npos) {
  pos = cur.find_first_of('"');
  cur.replace(pos + 1, cur.find_last_of('"') - pos - 1, (std::string)base);
 }
 res += cur + "\n";
}
in.close();

ofstream out(filename);
out << res;
out.close();

}
