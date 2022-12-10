#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <omp.h>
class SampleSort
{

public:
    // Generates Data
    static std::vector<int> generateData(int n)
    {
        std::vector<int> data;
        for (int i = 0; i < n; i++)
        {
            data.push_back(i);
        }
        data.shrink_to_fit();
        return data;
    }
    // Prints Vectors
    static void printVector(std::vector<int> v)
    {
        std::ostringstream vit;
        if (!v.empty())
        {
            std::copy(v.begin(), v.end() - 1, std::ostream_iterator<int>(vit, ", "));
            vit << v.back();
        }

        std::cout << vit.str() << std::endl;
    }

    // Randomizes Data by performing swaps
    static std::vector<int> randomizeData(std::vector<int> data)
    {
        int swapLocation = 0;
        int length = data.size();
        for (int i = 0; i < length; i++)
        {
            swapLocation = rand() % length;
            int temp = data[i];
            data[i] = data[swapLocation];
            data[swapLocation] = temp;
        }
        return data;
    }

    // Sample Sort
    // randomized is unsorted vector
    // int k is the minimum bucket size
    // int p is the number of processors
    static std::vector<int> sampleSort(std::vector<int> randomized, int k, int p)
    {
        // If less Processor than min bucket Size
        if (k < p)
        {
            throw std::invalid_argument("P needs to be less than k");
        }

        int n = randomized.size();
        // If Bucket is smaller then min bucket size run standard sort
        // Base Case
        if (n < k)
        {
            std::sort(randomized.begin(), randomized.end());
            return randomized;
        }
        // Reserve Space for sorted bucket
        //printf("%lu", randomized.size());
        std::vector<int> solved;

        // Chose Splitters
        int splitterCount = p - 1;
        std::vector<int> splitters(splitterCount);
        int newSize = n;

        for (int i = 0; i < splitterCount; i++)
        {
            int removeLocation = (int)rand() % newSize;
            splitters.at(i) = (randomized.at(removeLocation));
            randomized.erase(randomized.begin() + removeLocation);
            newSize--;
        }
        // Sort Splitters
        std::sort(splitters.begin(), splitters.end());

        // Sort elements into buckets around splitter
        std::vector<std::vector<int>> buckets(p);
        for (int e : randomized)
        {
            // If smaller than first splitter element insert into 0th bucket
            if (e < splitters.at(0))
            {
                buckets.at(0).push_back(e);
            }
            // If larger than last splitter element insert into last bucket
            else if (e >= splitters.at(splitters.size() - 1))
            {
                buckets.at(buckets.size() - 1).push_back(e);
            }
            // Else insert into bucket between splitter values
            else
            {

                for (int j = 1; j < splitters.size(); j++)
                {
                    if (splitters.at(j - 1) < e && e <= splitters.at(j))
                    {
                        buckets.at(j).push_back(e);
                    }
                }
            }
        }

        // Recursive Call on buckets
        std::vector<std::vector<int>> sortedBuckets;
        for (std::vector<int> b : buckets)
        {
            // Recursive Call
            b.shrink_to_fit();
            sortedBuckets.emplace_back(sampleSort(b, k, p));
        }

        // Reconstruction
        int i = 0;
        for (std::vector<int> b : sortedBuckets)
        {
            solved.insert(solved.end(), b.begin(), b.end());
            if (i != splitters.size())
            {
                solved.push_back(splitters.at(i));
            }
            i++;
        }
        
        return solved;
    }

    // Sample Sort
    // randomized is unsorted vector
    // int k is the minimum bucket size
    // int p is the number of processors
    static std::vector<int> sampleSortParallel(std::vector<int> randomized, int k, int p)
    {
        // If less Processor than min bucket Size
        if (k < p)
        {
            throw std::invalid_argument("P needs to be less than k");
        }

        int n = randomized.size();
        // If Bucket is smaller then min bucket size run standard sort
        // Base Case
        if (n < k)
        {
            std::sort(randomized.begin(), randomized.end());
            return randomized;
        }
        // Reserve Space for sorted bucket
        std::vector<int> solved;
        solved.reserve(randomized.size());

        // Chose Splitters
        int splitterCount = p - 1;
        std::vector<int> splitters(splitterCount);
        int newSize = n;

        for (int i = 0; i < splitterCount; i++)
        {
            int removeLocation = (int)rand() % newSize;
            splitters.at(i) = (randomized.at(removeLocation));
            randomized.erase(randomized.begin() + removeLocation);
            newSize--;
        }
        // Sort Splitters
        std::sort(splitters.begin(), splitters.end());

        // Sort elements into buckets around splitter
        std::vector<std::vector<int>> buckets(p);
        for (int e : randomized)
        {
            // If smaller than first splitter element insert into 0th bucket
            if (e < splitters.at(0))
            {
                buckets.at(0).push_back(e);
            }
            // If larger than last splitter element insert into last bucket
            else if (e >= splitters.at(splitters.size() - 1))
            {
                buckets.at(buckets.size() - 1).push_back(e);
            }
            // Else insert into bucket between splitter values
            else
            {

                for (int j = 1; j < splitters.size(); j++)
                {
                    if (splitters.at(j - 1) < e && e <= splitters.at(j))
                    {
                        buckets.at(j).push_back(e);
                    }
                }
            }
        }

        omp_set_num_threads(p);

        std::vector<std::vector<int>> sortedBuckets(buckets.size());

        // Recursive Call on buckets
        int i;
#pragma omp parallel shared(buckets) private(i)
#pragma omp for
        for ( i = 0; i < buckets.size();i++)
        {
            auto b = buckets.at(i);
            // Recursive Call
            b.shrink_to_fit();
            sortedBuckets.at(i) = (sampleSort(b, k, p));
        }

#pragma omp barrier
        // Reconstruction
        int j = 0;
        for (std::vector<int> b : sortedBuckets)
        {
            solved.insert(solved.end(), b.begin(), b.end());
            if (j != splitters.size())
            {
                solved.push_back(splitters.at(j));
            }
            j++;
        }
        // printVector(solved);
        return solved;
    }

    static bool isSolved(std::vector<int> v)
    {
        // If Vector is Empty it is Solved
        if (v.size() == 0)
        {
            return true;
        }

        // Comfirms Sorted in Ascending Order
        for (int i = 0; i < v.size() - 1; i++)
        {
            if (v.at(i) > v.at(i + 1))
            {
                return false;
            }
        }
        return true;
    }
};

int main()
{
    int size = 10000;
    std::vector<int> data = SampleSort::generateData(size);
    std::vector<int> randomized = SampleSort::randomizeData(data);

    int k = 12;
    int p = 6;
    clock_t start, end;

    start = clock();
    std::vector<int> solved = SampleSort::sampleSort(randomized, k, p);
    end = clock();

    double duration = ((double)end - start) / CLOCKS_PER_SEC;
    printf("Time taken to execute in seconds : %f\n", duration);

    start = clock();
    std::vector<int> solvedParallel = SampleSort::sampleSortParallel(randomized, k, p);
    end = clock();

    duration = ((double)end - start) / CLOCKS_PER_SEC;
    printf("Time taken to execute in seconds : %f\n", duration);

    SampleSort::printVector(solved);
    SampleSort::printVector(solvedParallel);
    printf("%s\n", SampleSort::isSolved(solved) ? "Solved" : "Not Solved");
    printf("%s\n", SampleSort::isSolved(solvedParallel) ? "Solved" : "Not Solved");

    return 0;
}