#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>
// #include <insht.h>
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

    // static bool solved(std::vector<int> data,std::vector<int> solved){
    //     for(int i =0;i<data.size();i++){
    //         if(data.at(i)!= solved.at(i)){
    //             return false;
    //         }
    //     }
    //     return true;
    // }
    static std::vector<int> sampleSort(std::vector<int> randomized, int k, int p)
    {

        if (k < p)
        {
            throw std::invalid_argument("P needs to be less than k");
        }

        // randomized.erase(randomized.begin() + 1);
        int n = randomized.size();
        if (n < k)
        {
            std::sort(randomized.begin(), randomized.end());
            return randomized;
        }

        std::vector<int> solved;
        solved.reserve(randomized.size());

        int splitterCount = p - 1;
        std::vector<int> splitters;
        int newSize = n;
        for (int i = 0; i < splitterCount; i++)
        {
            int removeLocation = (int)rand() % newSize;
            splitters.push_back(randomized[removeLocation]);
            randomized.erase(randomized.begin() + removeLocation);
            newSize--;
        }
        std::sort(splitters.begin(), splitters.end());
        // std::cout << "\n\n\n";
        // printVector(splitters);
        // std::cout << "\n\n\n";

        size_t _pSize = (size_t)p;
        // std::cout << "pain";
        std::vector<std::vector<int>> buckets(p);

        // buckets.resize(_pSize);

        // std::cout << "paint";
        size_t _randomizedSize = (size_t)randomized.size();

        for (auto bucket : buckets)
        {
            bucket.resize(randomized.size());
        }
        //  std::cout << "resizing";
        // buckets.resize(p);

        //  std::cout << "create buckets";
        for (int i = 0; i < randomized.size(); i++)
        {
            // std::cout << randomized.size();
            // std::cout << "started while";
            int val = randomized.at(i);
            //  std::cout << "found front";
            for (int j = 0; j < p; j++)
            {
                if ((val <= splitters[j]) || (j == p))
                {
                    // std::cout << "adding front";
                    // std::cout << front;
                    buckets[j].push_back(val);
                }
            }
        }
        //  std::cout << "split buckets";
        for (std::vector<int> b : buckets)
        {
            b.shrink_to_fit();
            std::vector<int> sortedB = sampleSort(b, k, p);
            solved.insert(solved.end(), sortedB.begin(), sortedB.end());
        }

        return solved;
    }
    static bool isSolved(std::vector<int> v)
    {
        for (int i = 0; i < v.size() - 1; i++)
        {
            if (v[i] > v[i + 1])
            {
                return false;
            }
        }
        return true;
    }
};

int main()
{
    std::vector<int> data = SampleSort::generateData(100);
    printf("%s\n", SampleSort::isSolved(data) ? "Solved" : "Not Solved");
    std::vector<int> randomized = SampleSort::randomizeData(data);
    // SampleSort::printVector(randomized);
    int k = 20;
    int p = 8;
    std::vector<int> solved = SampleSort::sampleSort(randomized, k, p);
    std::cout << "Done!\n\n";
    printf("%s\n", SampleSort::isSolved(solved) ? "Solved" : "Not Solved");
    return 0;
}