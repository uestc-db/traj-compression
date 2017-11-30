/* This software is developed by caoweiquan322 OR DynamicFatty.
 * All rights reserved.
 *
 * Author: caoweiquan322
 */

#include "DouglasPeuckerBatchSimplifier.h"
#include"psimpl/psimpl.h"
#include<vector>
#include"Helper.h"
#include<QDebug>
#include"DotsException.h"

DouglasPeuckerBatchSimplifier::DouglasPeuckerBatchSimplifier(QObject *parent) : QObject(parent)
{

}

void DouglasPeuckerBatchSimplifier::simplify(const QVector<double> x, const QVector<double> y,
                                             QVector<double> &ox, QVector<double> &oy, double minimumDistance)
{
    // Validates input parameters.
    Helper::checkPositive("Minimum distance", minimumDistance);
    Helper::checkIntEqual(x.count(), y.count());

    // Douglas-Peucker simplification.
    std::vector<double> zipped, output;
    zipped.resize(x.count()*2);
    for (int i=0; i<x.count(); ++i)
    {
        zipped[2*i] = x.at(i);
        zipped[2*i+1] = y.at(i);
    }
    output.resize(x.count()*2);
    psimpl::PolylineSimplification<2, std::vector<double>::iterator, std::vector<double>::iterator > ps;
    std::vector<double>::iterator reducedEnd = ps.DouglasPeucker(zipped.begin(), zipped.end(),
                                                                 minimumDistance, output.begin());
    //qDebug("Simplified number of points is %d", std::distance(output.begin(), reducedEnd)/2);
    Helper::checkIntEqual(std::distance(output.begin(), reducedEnd)%2, 0);

    // Copy output.
    ox.clear();
    oy.clear();
    for (std::vector<double>::iterator itr = output.begin(); itr != reducedEnd; ++itr)
    {
        ox.append(*itr);
        ++itr;
        oy.append(*itr);
    }
}

void DouglasPeuckerBatchSimplifier::simplifyByIndex(const QVector<double> x, const QVector<double> y,
                                                    QVector<int> &simplifiedIndex, double minimumDistance)
{
    QVector<double> ox, oy;
    simplify(x, y, ox, oy, minimumDistance);

    // Extract indices.
    simplifiedIndex.clear();
    double epsonal = 1e-15;
    int subIdx = 0;
    for (int i=0; i<x.count(); ++i)
    {
        if (qFabs(x.at(i)-ox.at(subIdx)) < epsonal && qFabs(y.at(i)-oy.at(subIdx)) < epsonal)
        {
            ++subIdx;
            simplifiedIndex.append(i);
            if (subIdx >= ox.count() && i != x.count()-1)
                DotsException(QString("Index %1 exceeds output size %2.").arg(subIdx).arg(ox.count())).raise();
        }
    }
    if (subIdx != ox.count())
        DotsException(QString("Expected %2 indices but got %1.").arg(subIdx).arg(ox.count())).raise();
}

