#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;


class Manacher {
    public:
        string s;
        vector<int> palindromes;

        void init(string inS) {
            s = "@";
            for(char c: inS){
                s += "#";
                s += c;
            }
            s+= "#$";


            palindromes.assign(s.length(), 0);

        }

        int runManacher(){
            int l=0;
            int r=0;

            int n = s.length();
            int longest = -1;

            for(int i=1;i<n-1;i++){
                int mirror = l+r-i;

                if (i < r){
                    palindromes[i] = min(r - i, palindromes[mirror]);
                }

                while(s[i+1+palindromes[i]] == s[i-1-palindromes[i]]){
                    palindromes[i] += 1;
                }

                if(i+palindromes[i] > r){
                    l = i - palindromes[i];
                    r = i + palindromes[i];
                }

                if(longest == -1 || palindromes[i] > palindromes[longest]){
                    longest = i;
                }
            }
            string toRet = "";
            for(int j=longest - palindromes[longest]; j<longest+palindromes[longest]+1;j++){
                if(s[j] != '#' && s[j] != '@' && s[j] != '$'){
                    toRet+=s[j];
                }
            }
            return toRet.size();
        }
};


int main() {
    Manacher m;
    string x;
    cout << "Type a string: ";
    cin >> x;
    m.init(x);
    cout << m.runManacher()<< endl;
    return 0;
}
