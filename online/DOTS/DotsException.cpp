/* This software is developed by caoweiquan322 OR DynamicFatty.
 * All rights reserved.
 *
 * Author: caoweiquan322
 */

#include "DotsException.h"
#include<QDebug>

DotsException::DotsException(QString message)
{
    this->message = message;
}

DotsException::DotsException(const DotsException &e)
{
    this->message = e.message;
}

QString DotsException::getMessage()
{
    return this->message;
}

