#include<iostream>
#include "gen/haha.pb.h"
#include<nlohmann/json.hpp>

using namespace std;

int main() {
    cout << "hello" << endl;
    User u;
    u.set_age(23);
    cout << u.has_birthyear() << " birth year " << u.birthyear() << endl;
    cout << u.has_address() << " address " << u.address().c_str() << endl;
    return 0;
}