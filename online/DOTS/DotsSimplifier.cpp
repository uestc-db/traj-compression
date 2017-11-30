/* This software is developed by caoweiquan322 OR DynamicFatty.
 * All rights reserved.
 *
 * Author: caoweiquan322
 */

#include "DotsSimplifier.h"
#include"Helper.h"
#include"DotsException.h"
#include<QVector>
#include<QtMath>

DotsSimplifier::DotsSimplifier(QObject *parent, DotsSimplifier *cascadeRoot) : QObject(parent)
{
    lssdTh = 10000.0;
    lssdUpperBound = lssdTh*2.0;
    maxVkSize = 1e6;
    resetInternalData();

    // Assign reference to the root DOTS simplifier.
    isCascadeRoot = (cascadeRoot == NULL);
    if (isCascadeRoot)
        cascadeRoot = this;
    pX = &(cascadeRoot->ptx);
    pY = &(cascadeRoot->pty);
    pT = &(cascadeRoot->ptt);
    pXSum = &(cascadeRoot->xSum);
    pYSum = &(cascadeRoot->ySum);
    pTSum = &(cascadeRoot->tSum);
    pX2Sum = &(cascadeRoot->x2Sum);
    pY2Sum = &(cascadeRoot->y2Sum);
    pT2Sum = &(cascadeRoot->t2Sum);
    pXTSum = &(cascadeRoot->xtSum);
    pYTSum = &(cascadeRoot->ytSum);
}

void DotsSimplifier::setParameters(double lssdTh, double k, int maxVkSize)
{
    this->lssdTh = lssdTh;
    lssdUpperBound = lssdTh*k;
    this->maxVkSize = maxVkSize;
}

void DotsSimplifier::resetInternalData()
{
    // Clear data points and internal statistics.
    ptx.clear();
    pty.clear();
    ptt.clear();
    ptIndex.clear();
    xSum.clear();
    ySum.clear();
    tSum.clear();
    x2Sum.clear();
    y2Sum.clear();
    t2Sum.clear();
    xtSum.clear();
    ytSum.clear();

    // Clear structures for DAG construction and optimization.
    vK.clear();
    vL.clear();
    terminated.clear();
    numTerminated = 0;
    pathK.clear();
    issed.clear();
    parents.clear();

    // Input/output queue position.
    simplifiedIndex.clear();
    inputCount = 0;
    outputCount = 0;

    // Finish flag.
    finished = false;
}

void DotsSimplifier::finish()
{
    if (!finished)
    {
        finished = true;
        // Run DAG search once again to finish the simplification work.
        directedAcyclicGraphSearch();
    }
}

double DotsSimplifier::getLssdThreshold()
{
    return this->lssdTh;
}

double DotsSimplifier::getAverageSED()
{
    if (!finished)
        DotsException("Calling getAverageSSED() is not allowed before finished feeding data.").raise();
    if (inputCount<1)
        DotsException("No data points in the containers.").raise();

    return qSqrt(issed.at(inputCount-1)/inputCount);
}

double DotsSimplifier::getMaxLSSD()
{
    if (!finished)
        DotsException("Calling getMaxLSSD() is not allowed before finished feeding data.").raise();
    if (inputCount<1)
        DotsException("No data points in the containers.").raise();

    // LSSD
    double ret = 0;
    int node = inputCount-1;
    do
    {
        int parentNode = parents.at(node);
        double lssd = issed.at(node)-issed.at(parentNode);
        if (ret < lssd)
            ret = lssd;
        node = parentNode;
    } while (node>0);

    return ret;
}

void DotsSimplifier::batchDots(const QVector<double> &x, const QVector<double> &y, const QVector<double> &t,
                               QVector<double> &ox, QVector<double> &oy, QVector<double> &ot,
                               double lssdThreshold)
{
    QVector<int> simplifiedIndex;
    batchDotsByIndex(x, y, t, simplifiedIndex, lssdThreshold);

    // Store output.
    ox.clear();
    oy.clear();
    ot.clear();
    Helper::slice(x, simplifiedIndex, ox);
    Helper::slice(y, simplifiedIndex, oy);
    Helper::slice(t, simplifiedIndex, ot);
}

void DotsSimplifier::batchDotsByIndex(const QVector<double> &x, const QVector<double> &y, const QVector<double> &t,
                                      QVector<int> &simplifiedIndex, double lssdThreshold)
{
    DotsSimplifier simplifier;
    // Set the simplification tolerance to 3km.
    simplifier.setParameters(lssdThreshold);
    int pointCount = x.count();
    int idx;
    simplifiedIndex.clear();

    for(int i=0; i<pointCount; ++i)
    {
        // Feed one point.
        simplifier.feedData(x.at(i), y.at(i), t.at(i));
        // Check if there's output data.
        if(simplifier.readOutputIndex(idx))
        {
            simplifiedIndex.append(idx);
//            qDebug("Input: %4d, output: %4d (%4d), Delay: %3d", i+1, simplifier.getSimplifiedIndex(ox.count()-1)+1,
//                   ox.count(), i-simplifier.getSimplifiedIndex(ox.count()-1));
        }
    }
//    qDebug("=====> Finished <=======");
    simplifier.finish();
    while (simplifier.readOutputIndex(idx)) {
        simplifiedIndex.append(idx);
//        qDebug("Input: %4d, output: %4d (%4d), Delay: %3d", pointCount, simplifier.getSimplifiedIndex(ox.count()-1)+1,
//               ox.count(), pointCount-1-simplifier.getSimplifiedIndex(ox.count()-1));
    }
//    qDebug("Average SED of the simplified trajectory is %.3f meters.", simplifier.getAverageSED());
    //    qDebug("Maximum LSSD is %.3f", simplifier.getMaxLSSD());
}

void DotsSimplifier::batchDotsCascade(const QVector<double> &x, const QVector<double> &y, const QVector<double> &t,
                                      QVector<double> &ox, QVector<double> &oy, QVector<double> &ot,
                                      double lssdThreshold)
{
    QVector<int> simplifiedIndex;
    batchDotsCascadeByIndex(x, y, t, simplifiedIndex, lssdThreshold);

    // Store output.
    ox.clear();
    oy.clear();
    ot.clear();
    Helper::slice(x, simplifiedIndex, ox);
    Helper::slice(y, simplifiedIndex, oy);
    Helper::slice(t, simplifiedIndex, ot);
}

void DotsSimplifier::batchDotsCascadeByIndex(const QVector<double> &x, const QVector<double> &y, const QVector<double> &t,
                                             QVector<int> &simplifiedIndex, double lssdThreshold)
{
    // Clear output.
    simplifiedIndex.clear();

    // Construct cascade simplifier.
    const double DEFAULT_START_THRESHOLD = 100.0;// Start by 100 meters as a threshold by default.
    double startThreshold = DEFAULT_START_THRESHOLD < lssdThreshold/8.0 ? DEFAULT_START_THRESHOLD : lssdThreshold/8.0;
    int cascadeCount = qFloor(qLn(lssdThreshold/startThreshold)/qLn(2.0))+1;
    double k = qPow(lssdThreshold/startThreshold, 1.0/(cascadeCount-1));
    double th = startThreshold;
    QObject *root = new QObject();
    QVector<DotsSimplifier *> cascade;
    for (int i=0; i<cascadeCount; ++i)
    {
        DotsSimplifier *s = new DotsSimplifier(root, i==0 ? NULL : cascade[0]);
        s->setParameters(th, k);
        th*=k;
        cascade.append(s);
    }

    // Run DOTS in cascade manner.
    int pointCount = x.count();
    double px, py, pt;
    DotsSimplifier *first = cascade[0];
    for (int i=0; i<pointCount; ++i)
    {
        px = x.at(i);
        py = y.at(i);
        pt = t.at(i);
        first->feedData(px, py, pt);
        int index = -1;
        if (first->readOutputIndex(index))
        {
            bool gotOutput = true;
            for (int j=1; j<cascadeCount; ++j)
            {
                DotsSimplifier *s = cascade[j];
                s->feedIndex(index);
                if (!(s->readOutputIndex(index)))
                {
                    gotOutput = false;
                    break;
                }
            }
            if (gotOutput)
            {
                simplifiedIndex.append(index);
            }
        }
    }
//    qDebug("Output count: %d", ox.count());
//    qDebug("=====> Finished <=======");

    // Finish simplifiers from front to end.
    for (int i=0; i<cascadeCount-1; ++i)
    {
        DotsSimplifier *s = cascade[i];
        DotsSimplifier *n = cascade[i+1];
        s->finish();
        int index = -1;
        while (s->readOutputIndex(index))
            n->feedIndex(index);
    }
    // Read output from the last simplifier.
    DotsSimplifier *last = cascade.last();
    last->finish();
    int index = -1;
    while (last->readOutputIndex(index))
    {
        simplifiedIndex.append(index);
    }

    // Destroy the total cascade.
    delete root;
}
