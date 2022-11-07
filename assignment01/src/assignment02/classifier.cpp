#include "classifier.hpp"
#include <utility>
#include <vector>
#include <algorithm>

float euclideanDist(cv::Point2f& p, cv::Point2f& q) {
    cv::Point2f diff = p - q;
    return cv::sqrt(diff.x*diff.x + diff.y*diff.y);
}


float pathNorm(const std::vector<cv::Point2f>& a, const std::vector<cv::Point2f>& b)
{
    if (a.size() != b.size()) {
        printf("a.size(): %d \n", a.size());
        printf("b.size(): %d \n", b.size());
        printf("\n\n\n\n\n\n\n\n\n");
    }

    assert(a.size() == b.size());

    float result = 0.0f;
    for(int i = 0; i < a.size(); ++i)
    {
        result += (a[i].x - b[i].x) * (a[i].x - b[i].x);
        result += (a[i].y - b[i].y) * (a[i].y - b[i].y);
    }

    return std::sqrt(result);
}

Classifier::Classifier(std::vector<Digit> dataSet, const int k)
: c_dataSet(std::move(dataSet))
, c_simplifiedSize(c_dataSet.front().points().size())
, c_k(k)
{
}

void previewPath(cv::Mat img, std::vector<cv::Point2f> path) {
    cv::Point2f center = cv::Point2f(250, 250);

    for (auto i = 0u; i + 1 < path.size(); ++i)
    {
        //printf("path[%d]: %f, %f \n", i, path[i].x, path[i].y);
        cv::line(img, path[i] * 200.0f + center, path[i+1] * 200.0f + center, cv::Scalar(0, 0, 255), 3);
    }
}

void Classifier::classify(const std::vector<cv::Point2f>& path)
{
    if (path.size() < 2) return;

    // equidistant sampling
    simplify(path);
    
    std::vector<cv::Point2f> normalizedPath = std::vector<cv::Point2f>();
    for (int i = 0; i < c_simplifiedSize; ++i) 
        normalizedPath.push_back(m_simplifiedPath[i]);

    normalize(normalizedPath);
    //flipY(m_simplifiedPath);

    std::vector<std::pair<int, float>> distances;
    distances.reserve(c_dataSet.size());

    
    auto img = cv::Mat(500, 500, CV_8UC3, cv::Scalar(0, 0, 0));
    previewPath(img, normalizedPath);

    // [ Digit1(points: [Point2f]), Digit2, Digit3, ... ]

    for (int i = 0; i < c_dataSet.size(); i++) {
        std::vector<cv::Point2f> castedDigit;

        for (cv::Point2i point : c_dataSet[i].points()) {
            castedDigit.push_back(cv::Point2f((float) point.x, (float) point.y));
        }

        normalize(castedDigit);
        flipY(castedDigit);

        //if (i == 1) previewPath(img, castedDigit);
        
        distances.push_back(std::make_pair(i, pathNorm(normalizedPath, castedDigit)));
    }

    cv::imshow("normalized", img);


    std::sort(distances.begin(), distances.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b) {
        return a.second < b.second;
    });

    std::vector<int> votes(10, 0);
    for (int i = 0; i < c_k; i++) {
        votes[c_dataSet[distances[i].first].label()]++;
    }

    m_result = std::max_element(votes.begin(), votes.end()) - votes.begin();
}

int Classifier::getResult() const
{
    return m_result;
}

std::vector<cv::Point2f> Classifier::getSimplifiedPath() const
{
    return m_simplifiedPath;
}

void Classifier::simplify(std::vector<cv::Point2f> path)
{
    assert(path.size() > 0);
    
    // calculate length of path
    float length = 0.0f;
    for(int i = 1; i < path.size(); ++i)
    {
        length += euclideanDist(path[i], path[i - 1]);
    }

    // calculate spacing
    const float spacing = length / (c_simplifiedSize - 1);
    
    // sample path
    m_simplifiedPath.clear();
    m_simplifiedPath.reserve(c_simplifiedSize);
    float currentSegmentStartLength = 0.0f;

    int currentSegment = 0;

    for (int i = 0; i < c_simplifiedSize; i++) {
        float samplePosition = i * spacing;

        float currentSegmentLength = euclideanDist(path[currentSegment], path[currentSegment + 1]);
        while (currentSegmentStartLength + currentSegmentLength < samplePosition && currentSegment < path.size() - 2) {
            currentSegment++;

            currentSegmentStartLength += currentSegmentLength;
            currentSegmentLength = euclideanDist(path[currentSegment], path[currentSegment + 1]);
        }

        float t = (samplePosition - currentSegmentStartLength) / currentSegmentLength;

        cv::Point2f sample = path[currentSegment] + t * (path[currentSegment + 1] - path[currentSegment]);
        m_simplifiedPath.push_back(sample);
    }
}

void Classifier::normalize(std::vector<cv::Point2f>& path)
{
    //std::vector<cv::Point2f> normalizedPath = std::vector<cv::Point2f>();

    // calculate bounding box
    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::min();

    for(const auto& point : path)
    {
        minX = std::min(minX, point.x);
        maxX = std::max(maxX, point.x);
        minY = std::min(minY, point.y);
        maxY = std::max(maxY, point.y);
    }

    cv::Rect2f boundingBox = cv::Rect2f(minX, minY, maxX - minX, maxY - minY);

    //printf("boundingBox: %f, %f, %f, %f \n", boundingBox.x, boundingBox.y, boundingBox.width, boundingBox.height);

    float scaleFactor = 1.0f / std::max(boundingBox.width, boundingBox.height);
    cv::Point2f center = boundingBox.tl() + 0.5f * cv::Point2f(boundingBox.width, boundingBox.height);

    // apply transformation
    for(int i = 0; i < path.size(); ++i)
    {
        cv::Point2f p = (path[i] - center) * scaleFactor;
        path[i] = p;
    }
    
    //path = normalizedPath;
}

void Classifier::flipY(std::vector<cv::Point2f>& path)
{
    for(int i = 0; i < path.size(); ++i)
    {
        path[i].y *= -1.0f;
    }
}
