#include "xor.h"
#include <chrono>

void TestStructuralXorImpl(uint64_t* input, uint64_t* output, int size)
{
    for (unsigned int i = 0; i < size; i += 1)
    {
        uint64_t h, h1, h2;

        h1 = input[i];
        h2 = 0x4cf5ad432745937full;

        h1 ^= h2;

        output[i] = h1;
    }
}

void TestSSE2XorImpl(uint64_t* input, uint64_t* output, int size)
{
    for (unsigned int i = 0; i < size; i += 2)
    {
        __m128i h, h1, h2;
        // Do not do like this - its slow!
        // h1 = _mm_set_epi64x(*(input + i + 1), *(input + i));
        // use load - it knows how many elements should be taken

        h2 = _mm_set1_epi64x(0x4cf5ad432745937full);

        h1 = _mm_xor_si128(h1, h2);

        _mm_store_si128((__m128i*)&output[i], h1);
    }
}

void TestAVXXorImpl(uint64_t* input, uint64_t* output, int size)
{   
    for (unsigned int i = 0; i < size; i += 4)
    {
        // unsupported types for integers
        // it would bring same output as sse2
    }
}

void TestAVX2XorImpl(uint64_t* input, uint64_t* output, int size)
{   
    for (unsigned int i = 0; i < size; i += 4)
    {
        __m256i h1, h2, h3;

        h1 = _mm256_set_epi64x(
            *(input + i + 3),
            *(input + i + 2),
            *(input + i + 1), 
            *(input + i)
        );

        h2 = _mm256_set1_epi64x(0x4cf5ad432745937full);

        h1 = _mm256_xor_si256(h1, h2);

        _mm256_store_si256((__m256i*)&output[i], h1);
    }
}

void TestAllXorImpl()
{
    ArrayGenerator * arrayGenerator = new ArrayGenerator();
    int size = 1000000;
    uint64_t* input = arrayGenerator->generateRandomUint64Array(size);
    uint64_t* output = new uint64_t[size];

    auto start = std::chrono::high_resolution_clock::now();
    TestStructuralXorImpl(input, output, size);
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    if (XOR_DEBUG) std::cout << output[0] << "\t" << output[1] << std::endl;
    std::cout << "Scalar\t   XOR " << size << " elements: " << elapsed.count() << " s" << std::endl;
    
    start = std::chrono::high_resolution_clock::now();
    TestSSE2XorImpl(input, output, size);
    finish = std::chrono::high_resolution_clock::now();
    elapsed = finish - start;
    if (XOR_DEBUG) std::cout << output[0] << "\t" << output[1] << std::endl;
    std::cout << "SSE2\t   XOR " << size << " elements: " << elapsed.count() << " s" << std::endl;
    
    start = std::chrono::high_resolution_clock::now();
    TestAVX2XorImpl(input, output, size);
    finish = std::chrono::high_resolution_clock::now();
    elapsed = finish - start;
    if (XOR_DEBUG) std::cout << output[0] << "\t" << output[1] << std::endl;
    std::cout << "AVX2\t   XOR " << size << " elements: " << elapsed.count() << " s" << std::endl;

    std::cout << std::endl;

    delete [] input;
    delete [] output;
}