/*
Example Scenario: Receive data from multiple servers all already in chronological order with respect to their own server
    -> When combined entries are sorted within each server, they are not necessarily sorted across all servers
    -> We want to sort all entries across all servers by timestamp in an efficient way
*/

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// stores entries from each server as their 3 key components: timestamp (main sorting feature), server_id, and the associated message
struct Entry {
    int timestamp;
    int server_id;
    string message;
};

// the main method and format for printing the pre-sorted and then post-sorted list
void print_entries(const vector<Entry>& entries) {
    for (const auto& entry: entries) {
        cout << "[" << entry.timestamp << "] " << entry.server_id << " : " << entry.message << '\n';
    }
    cout << '\n';
}

// insertion sort to make small runs larger for merge sort from left to right index -> creates sorted chunks
// idea: find each elements correct position within the left and right bounds
void insertion_sort(vector<Entry>& entries, int left, int right) {
    for (int i = left + 1; i <= right; i++) { // sorts only from left to right index 
        Entry key = entries[i]; // value to insert into correct position first
        int j = i - 1;
        while (j >= left && entries[j].timestamp > key.timestamp) { // move right until current key is less than next or right bound reached
            entries[j + 1] = entries[j];
            j--;
        }
        entries[j + 1] = key; // emplace in correct position
    }
}

// merge two pre-sorted runs into one: combine left->mid with mid+1->right
// fast on large pre-sorted chunks
void merge(vector<Entry>& entries, int left, int mid, int right) {
    // create copies of each run
    vector<Entry> left_part(entries.begin() + left, entries.begin() + mid + 1);
    vector<Entry> right_part(entries.begin() + mid + 1, entries.begin() + right + 1);
    int i = 0; // idx in left run
    int j = 0; // idx in right run
    int k = left; // idx to write to in original array
    while (i < left_part.size() && j < right_part.size()) { // while both runs have elements remaining
        if (left_part[i].timestamp <= right_part[j].timestamp) { // choose lower timestamp
            entries[k] = left_part[i]; // left run is smaller
            i++;
        } else {
            entries[k] = right_part[j]; // right run is smaller
            j++;
        }
        k++;
    }
    while (i < left_part.size()) { // left run had leftover items after right is empty, copy leftovers over as is
        entries[k] = left_part[i];
        i++;
        k++;
    }
    while (j < right_part.size()) { // right run had leftover items after left is empty, copy leftovers over as is
        entries[k] = right_part[j];
        j++;
        k++;
    }
}

// function that determines where ascending and decreasing runs exist within the data
// returns the start and end idx of all runs found
vector<pair<int, int>> find_runs(vector<Entry>& entries, int min_run) {
    vector<pair<int, int>> runs; // store all runs found
    int n = entries.size();
    int i = 0; // current position of run search within entries
    while (i < n) { 
        int start = i;
        if (i == n - 1) { // if last element, nothing to compare to
            i++;
        } else {
            if (entries[i].timestamp <= entries[i + 1].timestamp) { // if current timestamp is less than or equal to next -> start of ascending run
                while (i + 1 < n && entries[i].timestamp <= entries[i + 1].timestamp) { // as long as next is greater than previous keep going
                    i++;
                }
            } else { // start of a descending run
                while (i + 1 < n && entries[i].timestamp > entries[i + 1].timestamp) { // as long as next is less than previous keep going
                    i++;
                }
                reverse(entries.begin() + start, entries.begin() + i + 1); // reverse descending run since mergesort is designed for ascending runs only
            }
            i++;
        }
        int end = i - 1;
        if (end - start + 1 < min_run) { // check if this run is too small (small runs are inefficient)
            end = min(start + min_run - 1, n - 1); // enlarges the run to fit the minimum size 
            insertion_sort(entries, start, end); // uses insertion sort to sort the enlarged run -> fast for small sections
            i = end + 1;
        }
        runs.push_back({start, end}); // store this run
    }
    return runs;
}


// the main sorting function that combines partially sorted parts and efficiently merges them using runs
void tim_sort(vector<Entry>& entries) {
    const int MIN_RUN = 4; // sets the minimum length of a run to prevent inefficiency of small runs
    vector<pair<int, int>> runs = find_runs(entries, MIN_RUN); // find and store all runs within entries
    while (runs.size() > 1) { // loop through every run merging until only one run remains (fully sorted)
        vector<pair<int, int>> merged_runs; // store new merged runs from prior iterations through the loop
        for (int i = 0; i < runs.size(); i += 2) { // loop in steps of two -> handle two runs at a time (merging)
            if (i + 1 < runs.size()) { // check if there are two runs available to merge, if odd continue 
                int left = runs[i].first;
                int mid = runs[i].second;
                int right = runs[i + 1].second;
                merge(entries, left, mid, right); // merge the two runs using mergesort algorithm 
                merged_runs.push_back({left, right}); // store the new merged run
            } else {
                merged_runs.push_back(runs[i]); // handles the case where there was an odd run out and stores it as is
            }
        }
        runs = merged_runs; // replace old runs with the newly merged runs (updating the list for further iterations)
    }
}



int main() {
    int n;
    cin >> n; // the number of entries to be read in
    vector<Entry> entries; // the vector to store the entries read in
    for (int i = 0; i < n; i++) { // loop through reading in each line as an entry and storing it as appropriate
        Entry e;
        cin >> e.timestamp >> e.server_id >> e.message;
        entries.push_back(e);
    }
    print_entries(entries); // print the pre-sorted entries list
    cout << "Running TimSort Now ..." << '\n' << '\n';
    tim_sort(entries); // sort entries by timestamp using tim_sort algorithm
    print_entries(entries); // print out the final sorted entries list
    return 0;
}