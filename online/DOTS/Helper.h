/* This software is developed by caoweiquan322 OR DynamicFatty.
 * All rights reserved.
 *
 * Author: caoweiquan322
 */

#ifndef HELPER_H
#define HELPER_H

#include <QObject>
#include<QVector>
#include<QtMath>

/**
 * @brief The Helper class provides some utilities for the package. E.g. parameter checking, logging, etc.
 */
class Helper : public QObject
{
    Q_OBJECT
protected:
    /**
     * @brief Helper is the protected constructor to prevent from instancing this class.
     * @param parent is the parent of this QObject.
     */
    explicit Helper(QObject *parent = 0);

public:
    /**
     * @brief checkNotNullNorEmpty validates the specified value. A DotsException would be raised if value is null
     * or empty.
     * @param name is the name of the variable to check.
     * @param value is the value of the variable to check.
     */
    static void checkNotNullNorEmpty(QString name, QString value);

    /**
     * @brief checkIntEqual checks if the specified integers are equal or not. A DotsException would be raised if a does
     * not equal to b.
     * @param a is the 1st integer to check.
     * @param b is the 2nd integer to check.
     */
    static void checkIntEqual(int a, int b);

    /**
     * @brief checkPositive checks if the specified value is positive or not. A DotsException would be raised if value
     * is non-positive.
     * @param name is the name of the variable to check.
     * @param value is the value of the variable to check.
     */
    static void checkPositive(QString name, double value);

    /**
     * @brief limitVal truncates val to be within range [lb, ub].
     * @param val is the value to limit.
     * @param lb is the lower bound.
     * @param ub is the upper bound.
     * @return the truncated value.
     */
    static inline double limitVal(double val, double lb, double ub)
    {
        if (val<lb)
            val = lb;
        if (val>ub)
            val = ub;
        return val;
    }

    /**
     * @brief parseMOPSI parses a MOPSI data file. Reference to http://cs.joensuu.fi/mopsi for details of
     * MOPSI dataset.
     * @param fileName is the file name of MOPSI format.
     * @param x is the x values of trajectory points.
     * @param y is the y values of trajectory points.
     * @param t is the timestamps of trajectory points.
     */
    static void parseMOPSI(QString fileName, QVector<double> &x, QVector<double> &y, QVector<double> &t);

    /**
     * @brief parseGeoLife parses a GeoLife data file. Reference to Dr. Yu Zheng in MRA (Microsoft Research in Asia)
     * for details of GeoLife dataset.
     * @param fileName is the file name of MOPSI format.
     * @param x is the x values of trajectory points.
     * @param y is the y values of trajectory points.
     * @param t is the timestamps of trajectory points.
     */
    static void parseGeoLife(QString fileName, QVector<double> &x, QVector<double> &y, QVector<double> &t);

    /**
     * @brief mercatorProject does mercator projection on the longitude/latitude pairs.
     * @param longitude is the longitude of positions.
     * @param latitude is the latitude of positions.
     * @param x is the x values of positions.
     * @param y is the y values of positions.
     */
    static void mercatorProject(QVector<double> &longitude, QVector<double> &latitude, QVector<double> &x,
                                QVector<double> &y);

    /**
     * @brief normalizeData normalizes data array by sutracts values by the mean or the first value. Method behavior
     * is controlled by parameter byMean.
     * @param x is the data array
     * @param byMean specifies by which value to subtracts each element. Set it to true (DEFAULT) if use the mean
     * value and false if use the first value.
     */
    static void normalizeData(QVector<double> &x, bool byMean = true);

    /**
     * @brief range generates a vector that increasingly from value "from" to value "to" with increasing step "step".
     * @param from is the start point of the vector.
     * @param to is the end point of the vector.
     * @param step is the increasing step.
     * @return
     */
    template<class T>
    static QVector<T> range(T from, T to, T step)
    {
        QVector<T> v;
        while(from <= to)
        {
            v.append(from);
            from+=step;
        }
        return v;
    }

    /**
     * @brief slice retrieves a subset from v. The subset indices is specified by indices.
     * @param v the full set.
     * @param indices the subset indices.
     * @param ov the output subset.
     */
    template<class T>
    static void slice(const QVector<T> &v, const QVector<int> &indices, QVector<T> &ov)
    {
        ov.clear();
        ov.resize(indices.count());
        int itr = 0;
        foreach (int index, indices) {
            ov[itr++] = v.at(index);
        }
    }

    /**
     * @brief convert translate a vector of type A to a vector of type B. Note that type A and B should have similar
     * properties. Such as float&double is a good conversion.
     * @param v the vector to convert.
     * @return the converted vector.
     */
    template<class A, class B>
    static QVector<B> convert(const QVector<A> &v)
    {
        QVector<B> ov(v.count());
        int itr = 0;
        foreach (A ele, v) {
            ov[itr++] = (B)ele;
        }
        return ov;
    }

    /**
     * @brief angleDiff calculates the difference between two angles. The result is always within range (-PI, PI].
     * @param a angle a
     * @param b angle b
     * @return the difference within range (-PI, PI].
     */
    static inline double angleDiff(double a, double b)
    {
        static const double PI_2 = 2.0*M_PI;
        double diff = a-b;
        while (diff > M_PI)
            diff -= PI_2;
        while (diff <= -M_PI)
            diff += PI_2;
        return diff;
    }

    template<class T>
    static double sum(const QVector<T> &v)
    {
        T _sum = 0;
        foreach (T x, v) {
            _sum += x;
        }
        return _sum;
    }

    template<class T>
    static double max(const QVector<T> &v)
    {
        T _max = -Helper::INF;
        foreach (T x, v) {
            if (x > _max)
                _max = x;
        }
        return _max;
    }

    template<class T>
    static double min(const QVector<T> &v)
    {
        T _min = Helper::INF;
        foreach (T x, v) {
            if (x < _min)
                _min = x;
        }
        return _min;
    }

    template<class T>
    static double mean(const QVector<T> &v)
    {
        QVector<T> copy = v;
        qSort(copy);
        return copy.at(copy.count()/2);
    }

protected:
    /**
     * @brief MOPSI_DATETIME_FORMAT represents the datetime format of MOPSI dataset.
     */
    static const QString MOPSI_DATETIME_FORMAT;

    /**
     * @brief SCALE_FACTOR_PRECISION represents the precision for mercator projection.
     */
    static const double SCALE_FACTOR_PRECISION;

    static const double ZERO;
    static const double INF;

signals:

public slots:
};

#endif // HELPER_H
