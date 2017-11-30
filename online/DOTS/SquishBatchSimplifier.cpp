/* This software is developed by caoweiquan322 OR DynamicFatty.
 * All rights reserved.
 *
 * Author: caoweiquan322
 */

#include "SquishBatchSimplifier.h"
#include"Helper.h"
#include<QMap>
#include<QPair>
#include<QDebug>
#include<QtMath>
#include"DotsException.h"

const double SquishBatchSimplifier::SED_ACCURACY = 1000.0;

SquishBatchSimplifier::SquishBatchSimplifier(QObject *parent) : QObject(parent)
{

}

void SquishBatchSimplifier::simplify(const QVector<double> x, const QVector<double> y, const QVector<double> t,
                                     QVector<double> &ox, QVector<double> &oy, QVector<double> &ot,
                                     int bufferSize)
{
    // Validates input parameters.
    Helper::checkIntEqual(x.count(), y.count());
    Helper::checkIntEqual(x.count(), t.count());
    Helper::checkPositive("bufferSize-4", (double)(bufferSize-4));

    QVector<int> simplifiedIndex;
    simplifyByIndex(x, y, t, simplifiedIndex, bufferSize);
    Helper::slice(x, simplifiedIndex, ox);
    Helper::slice(y, simplifiedIndex, oy);
    Helper::slice(t, simplifiedIndex, ot);
}

void SquishBatchSimplifier::simplifyByIndex(const QVector<double> x, const QVector<double> y, const QVector<double> t,
                                            QVector<int> &outIndex, int bufferSize)
{
    // Validates input parameters.
    Helper::checkIntEqual(x.count(), y.count());
    Helper::checkIntEqual(x.count(), t.count());
    Helper::checkPositive("bufferSize-4", (double)(bufferSize-4));

    // Maintain the buffer.
    QMap<int, SpatioPoint> indexMap;
    QMap< QPair<int, int>, bool > buffer;
    int pointCount = x.count();
    int inf = 1L<<30;
    indexMap[0] = SpatioPoint(x.at(0), y.at(0), t.at(0), inf);
    buffer[qMakePair<int, int>(inf, 0)] = true;
    indexMap[1] = SpatioPoint(x.at(1), y.at(1), t.at(1), 0);
    for (int i=2; i<pointCount; ++i)
    {
        // Cache index of the last point in buffer.
        int lastKey = indexMap.lastKey();
        // Add one point in to the indexMap.
        indexMap[i] = SpatioPoint(x.at(i), y.at(i), t.at(i), 0);
        // Calculate the sed value.
        int sed = estimateLossingSED(indexMap, lastKey);
        // Since the sed is calculated, insert it to the priority buffer.
        indexMap[lastKey].SED += sed;
        buffer[qMakePair<int, int>(indexMap[lastKey].SED, lastKey)] = true;

        // Check if the buffer is full.
        if (buffer.count() >= bufferSize)
        {
            removeFirst(buffer, indexMap);
        }
    }

    // Clear output and dump the buffer to output.
    outIndex = indexMap.keys().toVector();
    qSort(outIndex);
}

int SquishBatchSimplifier::estimateLossingSED(const QMap<int, SpatioPoint> &indexMap, int i)
{
    // Retrieve 3 necessary points.
    QMap<int, SpatioPoint>::const_iterator itr = indexMap.constFind(i);
    if (itr == indexMap.constEnd())
        DotsException(QString("SED: Index %1 not found in the buffer.").arg(i)).raise();
    const SpatioPoint &curr = *itr;
    itr--;
    if (itr == indexMap.constEnd())
        DotsException(QString("SED: Previous element of (%1) not found in the buffer.").arg(i)).raise();
    const SpatioPoint &prev = *itr;
    itr+=2;
    if (itr == indexMap.constEnd())
        DotsException(QString("SED: Next element of (%1) not found in the buffer.").arg(i)).raise();
    const SpatioPoint &next = *itr;

    // Calculate SED.
    double k = (curr.t - prev.t)/(next.t-prev.t);
    double px = (1.0-k)*prev.x + k*next.x;
    double py = (1.0-k)*prev.y + k*next.y;
    double sed = qSqrt((curr.x-px)*(curr.x-px)+(curr.y-py)*(curr.y-py));
    return qRound(sed*SED_ACCURACY);
}

void SquishBatchSimplifier::removeFirst(QMap<QPair<int, int>, bool > &buffer, QMap<int, SpatioPoint> &indexMap)
{
    const QPair<int, int> &first = buffer.firstKey();

    // Retrieve 3 necessary points.
    QMap<int, SpatioPoint>::const_iterator itr = indexMap.constFind(first.second);
    if (itr == indexMap.constEnd())
        DotsException(QString("REMOVE: Index %1 not found in the buffer.").arg(first.second)).raise();
    int currIndex = itr.key();
    int currSED = (*itr).SED;
    itr--;
    if (itr == indexMap.constEnd())
        DotsException(QString("REMOVE: Previous element of (%1) not found in the buffer.").arg(first.second)).raise();
    int prevIndex = itr.key();
    int prevSED = (*itr).SED;
    itr+=2;
    if (itr == indexMap.constEnd())
        DotsException(QString("REMOVE: Next element of (%1) not found in the buffer.").arg(first.second)).raise();
    int nextIndex = itr.key();
    int nextSED = (*itr).SED;

    // Update previous key and next key.
    indexMap.remove(currIndex);
    buffer.remove(first);
    if (prevIndex!=0)
    {
        indexMap[prevIndex].SED += currSED;
        buffer.remove(qMakePair(prevSED, prevIndex));
        buffer[qMakePair(prevSED+currSED, prevIndex)] = true;
    }
    indexMap[nextIndex].SED += currSED;
    if (buffer.contains(qMakePair(nextSED, nextIndex)))
    {
        buffer.remove(qMakePair(nextSED, nextIndex));
        buffer[qMakePair(nextSED+currSED, nextIndex)] = true;
    }
}

