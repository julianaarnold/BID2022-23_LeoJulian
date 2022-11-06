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

void Classifier::classify(const std::vector<cv::Point2f>& path)
{
    if (path.size() < 2) return;

    // equidistant sampling
    simplify(path);
    
    std::vector<cv::Point2f> normalizedPath = getSimplifiedPath();
    normalize(normalizedPath);

    
    
    std::vector<std::pair<int, float>> distances;
    distances.reserve(c_dataSet.size());

    // [ Digit1(points: [Point2f]), Digit2, Digit3, ... ]

    for (int i = 0; i < c_dataSet.size(); i++) {
        std::vector<cv::Point2f> castedDigit;

        for (cv::Point2i point : c_dataSet[i].points()) {
            castedDigit.push_back(cv::Point2f((float) point.x, (float) point.y));
        }  
        
        distances.push_back(std::make_pair(i, pathNorm(normalizedPath, castedDigit)));
    }


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
    m_simplifiedPath.push_back(path.front());
    float currentLength = 0.0f;
    float lastSampleLength = 0.0f;

    for(int i = 1u; i < path.size(); ++i)
    {
        const float segmentLength = euclideanDist(path[i], path[i - 1]);
        currentLength += segmentLength;

        while (currentLength - lastSampleLength >= spacing)
        {
            //const float t = (spacing - (currentLength - segmentLength - lastSampleLength)) / segmentLength;
            m_simplifiedPath.push_back(path[i-1]); //path[i - 1] + t * (path[i] - path[i - 1]));
            lastSampleLength += spacing;
        }
    }
}

void Classifier::normalize(std::vector<cv::Point2f>& path)
{
    std::vector<cv::Point2f> normalizedPath = std::vector<cv::Point2f>();

    // calculate bounding box
    cv::Rect2f boundingBox;
    for(const auto& point : path)
    {
        boundingBox |= cv::Rect2f(point, point);
    }

    float scaleFactor = 1.0f / std::max(boundingBox.width, boundingBox.height);
    cv::Point2f center = boundingBox.tl() + 0.5f * cv::Point2f(boundingBox.width, boundingBox.height);

    // apply transformation and mirror y
    for(int i = 0; i < path.size(); ++i)
    {
        cv::Point2f p = (path[i] - center) * scaleFactor;
        p.y *= -1;

        normalizedPath.push_back(p);
    }
    
    path = normalizedPath;
}

