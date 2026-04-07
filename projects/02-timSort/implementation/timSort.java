/*
Example Scenario: Receive data from multiple servers all already in chronological order with respect to their own server
    -> When combined entries are sorted within each server, they are not necessarily sorted across all servers
    -> We want to sort all entries across all servers by timestamp in an efficient way
*/

import java.util.*;

// stores entries from each server as their 3 key components: timestamp (main sorting feature), server_id, and the associated message
class Entry {
    int timestamp;
    int server_id;
    String message;

    public Entry(int timestamp, int server_id, String message) {
        this.timestamp = timestamp;
        this.server_id = server_id;
        this.message = message;
    }
}

public class timSort {

    // the main method and format for printing the pre-sorted and then post-sorted list
    public static void printEntries(List<Entry> entries) {
        for (Entry entry : entries) {
            System.out.println("[" + entry.timestamp + "] " + entry.server_id + " : " + entry.message);
        }
        System.out.println();
    }

    // insertion sort to make small runs larger for merge sort from left to right index -> creates sorted chunks
    // idea: find each elements correct position within the left and right bounds
    public static void insertionSort(List<Entry> entries, int left, int right) {
        for (int i = left + 1; i <= right; i++) { // sorts only from left to right index
            Entry key = entries.get(i); // value to insert into correct position first
            int j = i - 1;
            // move right until current key is less than next or right bound reached
            while (j >= left && entries.get(j).timestamp > key.timestamp) {
                entries.set(j + 1, entries.get(j));
                j--;
            }
            entries.set(j + 1, key); // emplace in correct position
        }
    }

    // merge two pre-sorted runs into one: combine left->mid with mid+1->right
    // fast on large pre-sorted chunks
    public static void merge(List<Entry> entries, int left, int mid, int right) {
        // create copies of each run
        List<Entry> leftPart = new ArrayList<>(entries.subList(left, mid + 1));
        List<Entry> rightPart = new ArrayList<>(entries.subList(mid + 1, right + 1));

        int i = 0; // idx in left run
        int j = 0; // idx in right run
        int k = left; // idx to write to in original array

        // while both runs have elements remaining
        while (i < leftPart.size() && j < rightPart.size()) {
            if (leftPart.get(i).timestamp <= rightPart.get(j).timestamp) { // choose lower timestamp
                entries.set(k, leftPart.get(i)); // left run is smaller
                i++;
            } else {
                entries.set(k, rightPart.get(j)); // right run is smaller
                j++;
            }
            k++;
        }

        // left run had leftover items after right is empty, copy leftovers over as is
        while (i < leftPart.size()) {
            entries.set(k, leftPart.get(i));
            i++;
            k++;
        }

        // right run had leftover items after left is empty, copy leftovers over as is
        while (j < rightPart.size()) {
            entries.set(k, rightPart.get(j));
            j++;
            k++;
        }
    }

    // function that determines where ascending and decreasing runs exist within the data
    // returns the start and end idx of all runs found
    public static List<int[]> findRuns(List<Entry> entries, int minRun) {
        List<int[]> runs = new ArrayList<>(); // store all runs found
        int n = entries.size();
        int i = 0; // current position of run search within entries

        while (i < n) {
            int start = i;
            if (i == n - 1) { // if last element, nothing to compare to
                i++;
            } else {
                if (entries.get(i).timestamp <= entries.get(i + 1).timestamp) { // if current timestamp is <= next -> start of ascending run
                    while (i + 1 < n && entries.get(i).timestamp <= entries.get(i + 1).timestamp) { // as long as next is >= previous keep going
                        i++;
                    }
                } else { // start of a descending run
                    while (i + 1 < n && entries.get(i).timestamp > entries.get(i + 1).timestamp) { // as long as next is less than previous keep going
                        i++;
                    }
                    // reverse descending run since mergesort is designed for ascending runs only
                    Collections.reverse(entries.subList(start, i + 1));
                }
                i++;
            }

            int end = i - 1;
            if (end - start + 1 < minRun) { // check if this run is too small (small runs are inefficient)
                end = Math.min(start + minRun - 1, n - 1); // enlarges the run to fit the minimum size
                insertionSort(entries, start, end); // uses insertion sort to sort the enlarged run -> fast for small sections
                i = end + 1;
            }
            runs.add(new int[]{start, end}); // store this run
        }
        return runs;
    }

    // the main sorting function that combines partially sorted parts and efficiently merges them using runs
    public static void timSortAlgo(List<Entry> entries) {
        final int MIN_RUN = 4; // sets the minimum length of a run to prevent inefficiency of small runs
        List<int[]> runs = findRuns(entries, MIN_RUN); // find and store all runs within entries

        while (runs.size() > 1) { // loop through every run merging until only one run remains (fully sorted)
            List<int[]> mergedRuns = new ArrayList<>();
            for (int i = 0; i < runs.size(); i += 2) { // loop in steps of two -> handle two runs at a time (merging)
                if (i + 1 < runs.size()) { // check if there are two runs available to merge
                    int left = runs.get(i)[0];
                    int mid = runs.get(i)[1];
                    int right = runs.get(i + 1)[1];
                    merge(entries, left, mid, right); // merge the two runs using mergesort algorithm
                    mergedRuns.add(new int[]{left, right}); // store the new merged run
                } else {
                    mergedRuns.add(runs.get(i)); // handles the case where there was an odd run out
                }
            }
            runs = mergedRuns; // replace old runs with the newly merged runs
        }
    }

    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        if (!sc.hasNextInt()) return;
        
        int n = sc.nextInt(); // the number of entries to be read in
        List<Entry> entries = new ArrayList<>();

        for (int i = 0; i < n; i++) { // loop through reading in each line as an entry and storing it as appropriate
            int ts = sc.nextInt();
            int sid = sc.nextInt();
            String msg = sc.next();
            entries.add(new Entry(ts, sid, msg));
        }

        printEntries(entries); // print the pre-sorted entries list
        System.out.println("Running TimSort Now ...\n");

        timSortAlgo(entries); // sort entries by timestamp using tim_sort algorithm

        printEntries(entries); // print out the final sorted entries list
        sc.close();
    }
}