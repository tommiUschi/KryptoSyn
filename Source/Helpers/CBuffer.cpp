
#include "CBuffer.h"


CBuffer::CBuffer()
= default;

CBuffer::~CBuffer()
= default;


void CBuffer::Initialize(double sampleRate, float maxDelTimeMs)
{
    currentSampleRate = sampleRate;
    // calculation of the required samples
    auto samplesNeeded = static_cast<int>((maxDelTimeMs / 1000.0f) * currentSampleRate + 1.0f);
    // calculate the next power of two using std::ceil and std::log2
    bufferLen = static_cast<int>(std::pow(2, std::ceil(std::log2(samplesNeeded))));
    inxMask = bufferLen - 1;
    // C++20: automatic zero-initialization of the array
    delaybuffer = std::make_unique<float[]>(bufferLen);
    writeInd = 0;
}

float CBuffer::ReadBuffer(float delTimeMs) const noexcept
{
    jassert(delaybuffer != nullptr);

    auto totalDelSampl = static_cast<float>((delTimeMs / 1000.0f) * currentSampleRate);

    auto pastInd = static_cast<int>(std::floor(totalDelSampl));
    auto fragment = totalDelSampl - static_cast<float>(pastInd);

    int readIndA = (writeInd - 1 - pastInd) & inxMask;
    int readIndB = (readIndA - 1) & inxMask;

    float rightChan = delaybuffer[readIndA];
    float leftChan = delaybuffer[readIndB];

    return std::lerp(rightChan, leftChan, fragment);
}

float CBuffer::ReadSamples(int samplesDelay) const noexcept
{
    jassert(delaybuffer != nullptr);
    const int readInd = (writeInd - 1 - samplesDelay) & inxMask;
    return delaybuffer[readInd];
}

void CBuffer::Clear()
{
    if (delaybuffer != nullptr)
    {
        std::fill_n(delaybuffer.get(), bufferLen, 0.0f);
    }
}





