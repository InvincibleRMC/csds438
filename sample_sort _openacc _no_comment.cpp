#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <openacc.h>
class SampleSort
{

public:
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

    static std::vector<int> sampleSort(std::vector<int> randomized, int k, int p)
    {
        if (k < p)
        {
            throw std::invalid_argument("P needs to be less than k");
        }

        int n = randomized.size();
        if (n < k)
        {
            std::sort(randomized.begin(), randomized.end());
            return randomized;
        }
        std::vector<int> solved;

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
        std::sort(splitters.begin(), splitters.end());

        std::vector<std::vector<int>> buckets(p);
        for (int e : randomized)
        {if (e < splitters.at(0))
            {
                buckets.at(0).push_back(e);
            }
            else if (e >= splitters.at(splitters.size() - 1))
            {
                buckets.at(buckets.size() - 1).push_back(e);
            }
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

        std::vector<std::vector<int>> sortedBuckets;
        for (std::vector<int> b : buckets)
        {
            b.shrink_to_fit();
            sortedBuckets.emplace_back(sampleSort(b, k, p));
        }

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

     static std::vector<int> sampleSortParallel(std::vector<int> randomized, int k, int p)
    {
        if (k < p)
        {
            throw std::invalid_argument("P needs to be less than k");
        }

        int n = randomized.size();
        if (n < k)
        {
            std::sort(randomized.begin(), randomized.end());
            return randomized;
        }
        std::vector<int> solved;
        solved.reserve(randomized.size());

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
        std::sort(splitters.begin(), splitters.end());

        std::vector<std::vector<int>> buckets(p);
        for (int e : randomized)
        {
           if (e < splitters.at(0))
            {
                buckets.at(0).push_back(e);
            }
            else if (e >= splitters.at(splitters.size() - 1))
            {
                buckets.at(buckets.size() - 1).push_back(e);
            }
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

       

        std::vector<std::vector<int>> sortedBuckets(buckets.size());

      
        int i;
#pragma acc data copy(buckets)
    {
#pragma acc kernels
        for ( i = 0; i < buckets.size();i++)
        {
            auto b = buckets.at(i);
           
            b.shrink_to_fit();
            sortedBuckets.at(i) = (sampleSort(b, k, p));
        }
}
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
        return solved;
    }

    static bool isSolved(std::vector<int> v)
    {
        if (v.size() == 0)
        {
            return true;
        }

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
    int size = 10000000;
    std::vector<int> data = SampleSort::generateData(size);
    std::vector<int> randomized = SampleSort::randomizeData(data);

    int k = 10;
    int p = 8;
    clock_t start, end;

    start = clock();
    std::vector<int> solved = SampleSort::sampleSort(randomized, k, p);
    end = clock();

    double duration = ((double)end - start) / CLOCKS_PER_SEC;
    printf("Time taken to execute in seconds : %f\n", duration);

    
    start = clock();
    std::vector<int> solvedParallel = SampleSort::sampleSortParallel(randomized, k, p);
    end = clock();
     double duration = ((double)end - start) / CLOCKS_PER_SEC;
    printf("Time taken to execute in seconds : %f\n", duration);
    
   
    printf("%s\n", SampleSort::isSolved(solved) ? "Solved" : "Not Solved");
    printf("%s\n", SampleSort::isSolved(solvedParallel) ? "Solved" : "Not Solved");

    return 0;
}