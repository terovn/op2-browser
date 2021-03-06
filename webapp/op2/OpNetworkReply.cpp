/*======================================================== 
**University of Illinois/NCSA 
**Open Source License 
**
**Copyright (C) 2007-2008,The Board of Trustees of the University of 
**Illinois. All rights reserved. 
**
**Developed by: 
**
**    Research Group of Professor Sam King in the Department of Computer 
**    Science The University of Illinois at Urbana-Champaign 
**    http://www.cs.uiuc.edu/homes/kingst/Research.html 
**
**Permission is hereby granted, free of charge, to any person obtaining a 
**copy of this software and associated documentation files (the 
**��Software��), to deal with the Software without restriction, including 
**without limitation the rights to use, copy, modify, merge, publish, 
**distribute, sublicense, and/or sell copies of the Software, and to 
**permit persons to whom the Software is furnished to do so, subject to 
**the following conditions: 
**
*** Redistributions of source code must retain the above copyright notice, 
**this list of conditions and the following disclaimers. 
*** Redistributions in binary form must reproduce the above copyright 
**notice, this list of conditions and the following disclaimers in the 
**documentation and/or other materials provided with the distribution. 
*** Neither the names of <Name of Development Group, Name of Institution>, 
**nor the names of its contributors may be used to endorse or promote 
**products derived from this Software without specific prior written 
**permission. 
**
**THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
**EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
**MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
**IN NO EVENT SHALL THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR 
**ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
**TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
**SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE SOFTWARE. 
**========================================================== 
*/
#include "OpNetworkReply.h"
#include "OpNetworkAccessManager.h"
#include "OpNetwork.h"

#include <QDataStream>
#include <QVariant>
#include <QDebug>

using namespace OPNET;

OpNetworkReply::OpNetworkReply(int urlId, OpNetworkAccessManager* manager)
    : QNetworkReply(0)
    , m_reqId (urlId)
    , m_manager (manager)
    , m_state (Idle)
    , m_readIdx(0)
    , m_writeIdx(0)
{
    open(QIODevice::ReadOnly);
    m_readBuffer.open(QBuffer::ReadWrite);
}

OpNetworkReply::~OpNetworkReply()
{
    m_manager->removeReply(m_reqId);
}
void OpNetworkReply::abort()
{
    if (m_state != Finished) {
        m_state = Aborted;
        // send a message to network module
        OpNetwork::instance()->abortRequest(m_reqId);
    }
    m_manager->removeReply(m_reqId);
}

qint64 OpNetworkReply::readData(char* data, qint64 maxSize)
{
    m_readBuffer.seek(m_readIdx);
    if (m_readBuffer.bytesAvailable() == 0)
        return m_state == Aborted ? -1 : 0;
    if (maxSize == 1) {
        // just get one
        m_readBuffer.getChar(&data[0]);
        m_readIdx = m_readBuffer.pos();
        return 1;
    }

    maxSize = qMin<qint64> (maxSize, m_readBuffer.bytesAvailable());
    int ret = m_readBuffer.read(data, maxSize);
    m_readIdx = m_readBuffer.pos();
    return ret;
}

void OpNetworkReply::setInfo(QNetworkAccessManager::Operation op,
                             const QNetworkRequest& req)
{
    
    setOperation(op);
    setRequest(req);
    setUrl(req.url());
}

qint64 OpNetworkReply::bytesAvailable() const
{
    return QNetworkReply::bytesAvailable() + m_readBuffer.bytesAvailable();
}

void OpNetworkReply::feed(const QByteArray& data)
{
    if (data.size() == 0) {
        emit finished();
        m_state = Finished;
    } else {
        m_readBuffer.seek(m_writeIdx);
        m_readBuffer.write(data.constData(), data.size());
        m_writeIdx = m_readBuffer.pos();
        // set for read
        m_readBuffer.seek(m_readIdx);
        emit readyRead();
    }
}

void OpNetworkReply::feedMetaData(const QByteArray& metaData)
{
    QDataStream in(metaData);
    QVariant var;

    in >> var;
    QList<QVariant> attributes = var.toList();
    QList<QVariant>::const_iterator attrIter = attributes.constBegin();
    int attrCode = (int) QNetworkRequest::HttpStatusCodeAttribute;
    while (attrIter != attributes.constEnd()) {
        setAttribute((QNetworkRequest::Attribute) attrCode, *attrIter);
        attrCode ++;
        attrIter ++;
    }
    
    in >> var;
    QMap<QString, QVariant> headers = var.toMap();
    QMap<QString, QVariant>::const_iterator iter = headers.constBegin();
    while (iter != headers.constEnd()) {
        setRawHeader(iter.key().toLatin1(), iter.value().toByteArray());
        iter ++;
    }

    in >> var;
    NetworkError errorCode = (NetworkError) var.toInt();
    in >> var;
    QString errorString = var.toString();
    setError(errorCode, errorString);

    emit metaDataChanged();
}
