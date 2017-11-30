/* This software is developed by caoweiquan322 OR DynamicFatty.
 * All rights reserved.
 *
 * Author: caoweiquan322
 */

#include "PersistenceBatchSimplifier.h"
#include"Helper.h"
#include"DotsException.h"
#include<QtMath>
#include<QVector>
#include<QMap>
#include<QDebug>
#include<QSet>
#include<QList>

// Initialize the default BETA value.
const double PersistenceBatchSimplifier::DEFAULT_BETA = qDegreesToRadians(10.0);

// Initialize the default start point for cascade mode.
const double PersistenceBatchSimplifier::DEFAULT_DISTANCE_START = 2.0;

// Definition of MAX/MIN macros.
#define MAX(x, y) (x)>(y) ? (x) : (y)
#define MIN(x, y) (x)<(y) ? (x) : (y)

PersistenceBatchSimplifier::PersistenceBatchSimplifier(QObject *parent) : QObject(parent)
{

}

void PersistenceBatchSimplifier::simplify(const QVector<double> x, const QVector<double> y,
                                          QVector<double> &ox, QVector<double> &oy, double minimumDistance)
{
    // Validates input parameters.
    Helper::checkPositive("Minimum distance", minimumDistance);

    // Initialize input index.
    QVector<int> inIndex, outIndex;
    inIndex = Helper::range<int>(0, x.count()-1, 1);

    simplifyByIndex(x, y, inIndex, outIndex, minimumDistance);
    Helper::slice(x, outIndex, ox);
    Helper::slice(y, outIndex, oy);
}

void PersistenceBatchSimplifier::simplifyCascade(const QVector<double> x, const QVector<double> y,
                                                 QVector<double> &ox, QVector<double> &oy, double minimumDistance)
{
    // Validates input parameters.
    Helper::checkPositive("Minimum distance", minimumDistance);

    QVector<int> outIndex;
    simplifyByIndexCascade(x, y, outIndex, minimumDistance);
    Helper::slice(x, outIndex, ox);
    Helper::slice(y, outIndex, oy);
}

void PersistenceBatchSimplifier::simplifyByIndexCascade(const QVector<double> x, const QVector<double> y,
                                                 QVector<int> &outIndex, double minimumDistance)
{
    // Validates input parameters.
    Helper::checkPositive("Minimum distance", minimumDistance);

    // Calculate the necessary number of cascade for calculation.
    double startThreshold = MIN(DEFAULT_DISTANCE_START, minimumDistance/8.0);
    int numCascade = qFloor(qLn(minimumDistance/startThreshold)/qLn(2.0))+1;
    double k = qPow(minimumDistance/startThreshold, 1.0/(numCascade-1));
    double distance = startThreshold;

    // Initialize the sub index as full set at first.
    QVector<int> subIndex;
    subIndex = Helper::range<int>(0, x.count()-1, 1);

    // Do simplification in cascade.
    for (int i=0; i<numCascade; ++i)
    {
        QVector<int> localOutIndex;
        simplifyByIndex(x, y, subIndex, localOutIndex, distance);
        distance*=k;
        subIndex = localOutIndex;
        //qDebug("Size in round %3d: %5d", i, subIndex.count());
    }
    outIndex = subIndex;
}

void PersistenceBatchSimplifier::simplifyByIndex(const QVector<double> x, const QVector<double> y,
                                          const QVector<int> inIndex, QVector<int> &outIndex,
                                          double minimumDistance)
{
    Helper::checkIntEqual(x.count(), y.count());
    // We assume that inIndex is monotonously increasing and bounded by [0, #points].
    // As a result we omitted checking inIndex for efficiency considerations.

    // Clear output.
    outIndex.clear();

    // We refuse to process trajectories that is too short.
    if (inIndex.count() < 5)
    {
        outIndex = inIndex;
        return;
    }

    // Initialize curvature.
    QVector<double> sx, sy;
    Helper::slice(x, inIndex, sx);
    Helper::slice(y, inIndex, sy);
    QVector<double> curvature;
    int pointCount = inIndex.count();
    for (int i=0; i<pointCount-1; ++i)
    {
        curvature.append(qAtan2(sy.at(i+1)-sy.at(i), sx.at(i+1)-sx.at(i)));
    }
    curvature.append(0.0);// For the last point.
    for (int i=pointCount-1; i>0; --i)
    {
        curvature[i] = Helper::angleDiff(curvature.at(i), curvature.at(i-1));
    }
    curvature[0] = 0.0;

    // Detect local minimum and maximum.
    QVector<int> maxima, minima;
    double c1, c2, c3;
    if (curvature.at(0) > curvature.at(1))
        maxima.append(0);
    else if (curvature.at(0) <= curvature.at(1))
        minima.append(0);
    for (int i=1; i<pointCount-1; ++i)
    {
        c1 = curvature.at(i-1);
        c2 = curvature.at(i);
        c3 = curvature.at(i+1);
        if (c2>=c1 && c2>c3)
            maxima.append(i);
        else if (c2<c1 && c2<=c3)
            minima.append(i);
    }
    if (curvature.at(pointCount-1) > curvature.at(pointCount-2))
        maxima.append(pointCount-1);
    else if (curvature.at(pointCount-1) <= curvature.at(pointCount-2))
        minima.append(pointCount-1);
    //qDebug("#maxima: %d, #minima: %d", maxima.count(), minima.count());

    // Construct connected components.
    QVector<int> ccLeft, ccRight;
    QVector<bool> ccTerminated, ccPaused;
    int numTerminated = 0;
    QMap<int, int> index2Maxima;
    QVector< QVector<int> > ccInMaxima(maxima.count());
    QSet<int> betaPersistence;
    foreach (int index, minima) {
        ccLeft.append(index);
        ccRight.append(index);
        ccTerminated.append(false);
        ccPaused.append(false);
        //qDebug("minima: %d", index);
    }
    for (int i=0; i<maxima.count(); ++i)
        index2Maxima[maxima.at(i)] = i;

    int ccCount = minima.count();
    while(numTerminated < ccCount)
    {
        for (int i=0; i<ccCount; ++i)
        {
            if (!ccTerminated.at(i) && !ccPaused[i])
            {
                int a = ccLeft.at(i);
                int b = ccRight.at(i);
                int expandIndex = -1;
                if (a<=0 && b>=pointCount-1)
                {
                    ccTerminated[i] = true;
                    ++numTerminated;
                    betaPersistence.insert(minima.at(i));
                }
                else if (a<=0 && b<pointCount-1)
                {
                    // Expand right.
                    expandIndex = b+1;
                    ccRight[i] = expandIndex;
                }
                else if (a>0 && b>=pointCount-1)
                {
                    // Expand left.
                    expandIndex = a-1;
                    ccLeft[i] = expandIndex;
                }
                else
                {
                    // Choose left or right.
                    if (curvature.at(a-1) <= curvature.at(b+1))
                    {
                        expandIndex = a-1;
                        ccLeft[i] = expandIndex;
                    }
                    else
                    {
                        expandIndex = b+1;
                        ccRight[i] = expandIndex;
                    }
                }

                // CC merging.
                if (expandIndex>=0)
                {
                    if (index2Maxima.contains(expandIndex))
                    {
                        int mp = index2Maxima[expandIndex];
                        ccInMaxima[mp].append(i);
                        int numCC = ccInMaxima.at(mp).count();
                        if (expandIndex==0 || expandIndex==pointCount-1)
                        {
                            // Is an edge, yield.
                        }
                        else if (numCC == 2)
                        {
                            int cc1 = ccInMaxima.at(mp).at(0);
                            int cc2 = i;
                            if (minima.at(cc1) < minima.at(cc2))
                            {
                                // Merge cc2 to cc1
                                ccPaused[cc1] = false;
                                ccTerminated[cc2] = true;
                                ++numTerminated;
                                ccLeft[cc1] = MIN(ccLeft.at(cc1), ccLeft.at(cc2));
                                ccRight[cc1] = MAX(ccRight.at(cc1), ccRight.at(cc2));
                                if (curvature.at(expandIndex)-curvature.at(minima.at(cc2)) > DEFAULT_BETA)
                                {
                                    betaPersistence.insert(expandIndex);
                                    betaPersistence.insert(minima.at(cc2));
                                }
                            }
                            else
                            {
                                // Merge cc1 to cc2
                                ccTerminated[cc1] = true;
                                ccPaused[cc2] = false;
                                ++numTerminated;
                                if (curvature.at(expandIndex)-curvature.at(minima.at(cc1)) > DEFAULT_BETA)
                                {
                                    betaPersistence.insert(expandIndex);
                                    betaPersistence.insert(minima.at(cc1));
                                }
                                ccLeft[cc2] = MIN(ccLeft.at(cc1), ccLeft.at(cc2));
                                ccRight[cc2] = MAX(ccRight.at(cc1), ccRight.at(cc2));
                            }
                        }
                        else if (numCC > 2)
                        {
                            DotsException("#CCs assigned to one maxima should never exceed 2.").raise();
                        }
                        else // numCC < 2
                        {
                            ccPaused[i] = true;
                        }
                    }
                }
            }
        }
    }
    //qDebug("Finished phase 1, betaPersistence has %d elements.", betaPersistence.count());

    // Enumerating beta-persistence.
    QList<int> betaList = betaPersistence.toList();
    qSort(betaList.begin(), betaList.end());
    bool firstIsMinima = minima.count() > maxima.count();
    bool lastIsMinima = minima.count() > maxima.count();
    int firstIdx = betaList.first(), lastIdx = betaList.last();
    for (int i=0; i<MIN(maxima.count(), minima.count()); ++i)
    {
        if (maxima.at(i) == firstIdx)
        {
            firstIsMinima = false;
            break;
        }
        else if (minima.at(i) == firstIdx)
        {
            firstIsMinima = true;
            break;
        }
    }
    for (int i=0; i<MIN(maxima.count(), minima.count()); ++i)
    {
        if (maxima.at(maxima.count()-1-i) == lastIdx)
        {
            lastIsMinima = false;
            break;
        }
        else if (minima.at(minima.count()-1-i) == lastIdx)
        {
            lastIsMinima = true;
            break;
        }
    }
    if (firstIdx != 0)
    {
        if (firstIsMinima)
        {
            int extra = 0;
            int idx = -1;
            for(int i=0; i<maxima.count(); ++i)
            {
                idx = maxima.at(i);
                if (idx>firstIdx)
                    break;
                if (curvature.at(idx) > curvature.at(extra))
                    extra = idx;
            }
            if (extra != 0)
                betaList.prepend(extra);
            betaList.prepend(0);
        }
        else
        {
            int extra = 0;
            int idx = -1;
            for(int i=0; i<minima.count(); ++i)
            {
                idx = minima.at(i);
                if (idx>firstIdx)
                    break;
                if (curvature.at(idx) < curvature.at(extra))
                    extra = idx;
            }
            if (extra != 0)
                betaList.prepend(extra);
            betaList.prepend(0);
        }
    }
    if (lastIdx != pointCount-1)
    {
        if (lastIsMinima)
        {
            int extra = pointCount-1;
            int idx = -1;
            for(int i=maxima.count()-1; i>=0; --i)
            {
                idx = maxima.at(i);
                if (idx<lastIdx)
                    break;
                if (curvature.at(idx) > curvature.at(extra))
                    extra = idx;
            }
            if (extra != pointCount-1)
                betaList.append(extra);
            betaList.append(pointCount-1);
        }
        else
        {
            int extra = pointCount-1;
            int idx = -1;
            for(int i=minima.count()-1; i>=0; --i)
            {
                idx = minima.at(i);
                if (idx<lastIdx)
                    break;
                if (curvature.at(idx) < curvature.at(extra))
                    extra = idx;
            }
            if (extra != pointCount-1)
                betaList.append(extra);
            betaList.append(pointCount-1);
        }
    }
    //qDebug("Finished phase 2, betaPersistence has %d elements.", betaList.count());

    // Remove points that are too close.
    outIndex.append(inIndex.at(0));// We include point-0 without checking.
    int currentIdx = 1;
    int nextIdx = -1;
    double distance = 0.0;
    minimumDistance = minimumDistance*minimumDistance;
    for (int i=currentIdx+1; i<betaList.count(); ++i)
    {
        nextIdx = betaList.at(i);
        distance = qPow(sy.at(nextIdx)-sy.at(currentIdx), 2.0)+qPow(sx.at(nextIdx)-sx.at(currentIdx), 2.0);
        if (distance<minimumDistance)
        {
            if (qFabs(curvature.at(nextIdx)) > qFabs(curvature.at(currentIdx)))
                currentIdx = nextIdx;
        }
        else
        {
            outIndex.append(inIndex.at(currentIdx));
            currentIdx = nextIdx;
        }
    }
    if (outIndex.last() != inIndex.at(betaList.last())) // Append the last index if necessary.
        outIndex.append(inIndex.at(betaList.last()));
}


