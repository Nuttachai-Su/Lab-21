#include <iostream>
#include <cstdlib>

using namespace std;

int main(int avg,char *avp[]){
    if(avg == 1) {cout << "Please input numbers to find average.";}
    else {
        double sum = 0;
        double AVG = 0;
        for(int i=1;i<avg;i++){
            sum += atoi(avp[i]);
        }
        AVG = sum/(avg-1);
        cout << "---------------------------------\n";
        cout << "Average of "<< avg-1 << " numbers = " << AVG << "\n";
        cout << "---------------------------------\n";
    }
}
