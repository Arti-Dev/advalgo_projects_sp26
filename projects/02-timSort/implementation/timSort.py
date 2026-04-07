"""
Example Scenario: Receive data from multiple servers all already in chronological order with respect to their own server
    -> When combined entries are sorted within each server, they are not necessarily sorted across all servers
    -> We want to sort all entries across all servers by timestamp in an efficient way
"""

# stores entries from each server as their 3 key components: timestamp (main sorting feature), server_id, and the associated message
class Entry:
    def __init__(self, timestamp, server_id, message):
        self.timestamp = timestamp
        self.server_id = server_id
        self.message = message

# the main method and format for printing the pre-sorted and then post-sorted list
def print_entries(entries):
    for entry in entries:
        print(f"[{entry.timestamp}] {entry.server_id} : {entry.message}")
    print()

# insertion sort to make small runs larger for merge sort from left to right index -> creates sorted chunks
# idea: find each elements correct position within the left and right bounds
def insertion_sort(entries, left, right):
    for i in range(left + 1, right + 1):
        key = entries[i]
        j = i - 1
        while j >= left and entries[j].timestamp > key.timestamp:
            entries[j + 1] = entries[j]
            j -= 1
        entries[j + 1] = key

# merge two pre-sorted runs into one: combine left->mid with mid+1->right
# fast on large pre-sorted chunks
def merge(entries, left, mid, right):
    # create copies of each run
    left_part = entries[left:mid + 1]
    right_part = entries[mid + 1:right + 1]

    # idx for left run, right run
    i = j = 0

    # idx to write to in original array
    k = left
    
    # while both runs have elements remaining
    while i < len(left_part) and j < len(right_part):
        # choose lower timestamp
        if left_part[i].timestamp <= right_part[j].timestamp:
            entries[k] = left_part[i]
            i += 1
        else:
            entries[k] = right_part[j]
            j += 1
        k += 1
    
    while i < len(left_part):
        entries[k] = left_part[i]
        i += 1
        k += 1
    
    while j < len(right_part):
        entries[k] = right_part[j]
        j += 1
        k += 1

# function that determines where ascending and decreasing runs exist within the data
# returns the start and end idx of all runs found
def find_runs(entries, min_run):
    # store all runs found
    runs = []
    n = len(entries)
    # current position of run search within entries
    i = 0

    while i < n:
        start = i

        # if last element, nothing to compare to
        if i == n - 1:
            i+=1

        else:
            # if current timestamp is less than or equal to next -> start of ascending run
            if entries[i].timestamp <= entries[i + 1].timestamp:
                # as long as next is greater than previous keep going
                while i < n - 1 and entries[i].timestamp <= entries[i + 1].timestamp:
                    i += 1
            # else start of decreasing run
            else:
                # as long as next is less than previous keep going
                while i < n - 1 and entries[i].timestamp > entries[i + 1].timestamp:
                    i += 1
                # reverse descending run since mergesort is designed for ascending runs only
                entries[start:i + 1] = reversed(entries[start:i + 1])
                
            i += 1

        
        end = i - 1

        # check if the run is too small
        if end - start + 1 < min_run:
            # enlarge the run to fit the min size
            end = min(start + min_run - 1, n - 1)
            # use insertion sort for the this small run as it's efficient for small sections
            insertion_sort(entries, start, end)
            i = end + 1
        runs.append((start, end))
    
    return runs

# the main sorting function that combines partially sorted parts and efficiently merges them using runs
def tim_sort(entries):
    # sets the minimum length of a run to prevent inefficiency of small runs
    min_run = 4

    # find and store all runs within entries
    runs = find_runs(entries, min_run)

    # iteratively merge runs together until only one run remains (the whole sorted array)
    while len(runs) > 1:
        merged_runs = []

        # loop in steps of two -> handle two runs at a time (merging)
        for i in range(0, len(runs) - 1, 2):
            left = runs[i][0]
            mid = runs[i][1]
            right = runs[i + 1][1]

            # merge the two runs
            merge(entries, left, mid, right)
            # store the new merged run
            merged_runs.append((left, right))
        
        # handles the case where there was an odd run out and stores it as is
        if len(runs) % 2 == 1:
            merged_runs.append(runs[-1]) 
        
        # replace old runs with the newly merged runs (updating the list for further iterations)
        runs = merged_runs

if __name__ == "__main__":
    # number of entries to be sorted
    n = int(input())
    entries = []

    # loop through reading in each line as an entry and storing it as appropriate
    for _ in range(n):
        timestamp, server_id, message = input().split()
        entries.append(Entry(int(timestamp), int(server_id), message))

    # print the pre-sorted entries list
    print_entries(entries)

    print("Running TimSort Now ...\n")

    # sort entries by timestamp using tim_sort algorithm
    tim_sort(entries)

    # print out the final sorted entries list
    print_entries(entries)