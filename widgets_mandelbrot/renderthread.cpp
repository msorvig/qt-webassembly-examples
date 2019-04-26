/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "renderthread.h"

#include <QImage>
#include <QDebug>
#include <QThreadPool>
#include <cmath>

// runs function on a thread from the global QThreadPool, returns a
// function that can be called to vait for the function to finish.
std::function<void()> runOnThread(std::function<void()> fn)
{
    // This function starts a thread and is asynchronous. Allocate
    // state/context which whill keep track of the running thread.
    struct WaitContext {
        bool done = false;
        QMutex mutex;
        QWaitCondition threadDone;
    };
    WaitContext *context = new WaitContext();
    
    // runs function and signals the waiter when done
    auto threadFn = [=](){
        fn();
        QMutexLocker lock(&context->mutex);
        context->done = true;
        context->threadDone.wakeOne();
    };
    
    // wait for done
    auto waitFn = [=](){
        {
            QMutexLocker lock(&context->mutex);
            if (context->done)
                return;
            context->threadDone.wait(&context->mutex);
        }
        delete context;
    };
    
    // acquire a thread from the global thread pool
    class Runnable : public QRunnable
    {
    public:
        Runnable(const std::function<void()> &fn) :m_fn(fn) { }
        void run() override  { m_fn(); }
        std::function<void()> m_fn;
    };
    QThreadPool::globalInstance()->start(new Runnable(threadFn));

    return waitFn;
}

// calls the provided \a function  \a iterationCount times, using at maximum \a maxThreads thrads.
void runOnThreads(std::function<void(int i)> fn, int iterationCount, 
                  int maxThreads = QThread::idealThreadCount())
{
    // thread-safe current iteration counter
    QMutex mutex;
    int currentIteration = 0;
    auto nextIteration = [&mutex, &currentIteration]() {
        QMutexLocker locker(&mutex);
        return currentIteration++;
    };
    
    // thread function which acquires and runs iterations, until none left
    auto threadFn = [=]() {
        while(true) {
            int i = nextIteration();
            if (i >= iterationCount)
                return;
            fn(i);
        }
    };

    // start and wait for worker threads
    const int threadPackSize = qMin(iterationCount, maxThreads);
    QVector<std::function<void()>> waitFns;
    for (int i = 0; i < threadPackSize; ++i)
        waitFns.append(runOnThread(threadFn));
    for (int i = 0; i < threadPackSize; ++i)
        waitFns[i]();
}

// generates \a count results using the provided \function, using at most \a maxThreads threads.
template <typename Fn>
auto generate(const Fn &function, int count, int maxThreads = QThread::idealThreadCount()) -> QVector<decltype(function(1))>
{
    QVector<decltype(function(1))> results;
    results.resize(count);
    auto itFn = [&](int i) { results[i] = function(i); };
    runOnThreads(itFn, count, maxThreads);
    return results;
}

MandelbrotRenderer::MandelbrotRenderer(int tileCount, int threadCount, std::function<bool()> shouldAbort, std::function<void(const QImage &, double)> imageReady)
{
    this->tileCount = tileCount;
    this->threadCount = threadCount;
    this->shouldAbort = shouldAbort;
    this->imageReady = imageReady;
    for (int i = 0; i < ColormapSize; ++i)
        colormap[i] = rgbFromWaveLength(380.0 + (i * 400.0 / ColormapSize));
}

MandelbrotRenderer::RenderPassResult MandelbrotRenderer::renderTile(int tile, int pass, double centerX, double centerY, double scaleFactor, QImage *image)
{
    QSize imageSize = image->size();
    int halfWidth = imageSize.width() / 2;
    int halfHeight = imageSize.height() / 2;

    const int MaxIterations = (1 << (2 * pass + 6)) + 32;
    const int Limit = 4;
    bool allBlack = true;
    int beginY = tileBegin(tile, imageSize.height()) - halfHeight;
    int endY = tileEnd(tile, imageSize.height()) - halfHeight;
    
    // qDebug() << "redner tile" << tile << tileBegin(tile, imageSize.height()) << beginY << endY << imageSize.height() << (beginY  + halfHeight) << (endY -1  + halfHeight) << QThread::currentThread();

    for (int y = beginY; y < endY; ++y) {
        if (shouldAbort())
            return Abort;

        uint *scanLine =
                reinterpret_cast<uint *>(image->scanLine(y + halfHeight));
        double ay = centerY + (y * scaleFactor);

        for (int x = -halfWidth; x < halfWidth; ++x) {
            double ax = centerX + (x * scaleFactor);
            double a1 = ax;
            double b1 = ay;
            int numIterations = 0;

            do {
                ++numIterations;
                double a2 = (a1 * a1) - (b1 * b1) + ax;
                double b2 = (2 * a1 * b1) + ay;
                if ((a2 * a2) + (b2 * b2) > Limit)
                    break;

                ++numIterations;
                a1 = (a2 * a2) - (b2 * b2) + ax;
                b1 = (2 * a2 * b2) + ay;
                if ((a1 * a1) + (b1 * b1) > Limit)
                    break;
            } while (numIterations < MaxIterations);

            if (numIterations < MaxIterations) {
                *scanLine++ = colormap[numIterations % ColormapSize];
                allBlack = false;
            } else {
                *scanLine++ = qRgb(0, 0, 0);
            }
        }
    }
    return allBlack ? AllBlack : Done;
}

MandelbrotRenderer::RenderPassResult MandelbrotRenderer::renderPass(int pass, double centerX, double centerY, double scaleFactor, QImage *image)
{
    // Render the current pass in parallel. The return value from renderTile
    auto tileFn = [=](int tile) -> MandelbrotRenderer::RenderPassResult {
        return renderTile(tile, pass, centerX, centerY, scaleFactor, image);
    };
    QVector<MandelbrotRenderer::RenderPassResult> results = generate(tileFn, tileCount, threadCount);
    
    // Process result states
    bool allBlack = true;
    for (MandelbrotRenderer::RenderPassResult result : results) {
        switch (result) {
        case Done:
            allBlack = false;
            break;
        case AllBlack:
            break;
        case Abort:
            return Abort;
        }
    }
    return allBlack ? AllBlack : Done;
}

void MandelbrotRenderer::render(double centerX, double centerY, double scaleFactor, QSize resultSize, double resultDevicePixelRatio)
{
    resultSize *= resultDevicePixelRatio;
    centerX *= resultDevicePixelRatio;
    centerY *= resultDevicePixelRatio;
    
    // qDebug() << "MandelbrotRenderer::render" << resultSize;

    QImage image(resultSize, QImage::Format_RGB32);
    image.setDevicePixelRatio(resultDevicePixelRatio);

    const int NumPasses = 3;
    int pass = 2;
    while (pass < NumPasses) {
        MandelbrotRenderer::RenderPassResult result = renderPass(pass, centerX, centerY, scaleFactor, &image);
        switch (result) {
        case Done:
            imageReady(image, scaleFactor);
            ++pass;
            break;
        case AllBlack:
            if (pass == 0)
                pass = 4;
            else
                ++pass;
            break;
        case Abort:
            return;
        }
    } // while (pass < numPasses)
}

uint MandelbrotRenderer::rgbFromWaveLength(double wave)
{
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;

    if (wave >= 380.0 && wave <= 440.0) {
        r = -1.0 * (wave - 440.0) / (440.0 - 380.0);
        b = 1.0;
    } else if (wave >= 440.0 && wave <= 490.0) {
        g = (wave - 440.0) / (490.0 - 440.0);
        b = 1.0;
    } else if (wave >= 490.0 && wave <= 510.0) {
        g = 1.0;
        b = -1.0 * (wave - 510.0) / (510.0 - 490.0);
    } else if (wave >= 510.0 && wave <= 580.0) {
        r = (wave - 510.0) / (580.0 - 510.0);
        g = 1.0;
    } else if (wave >= 580.0 && wave <= 645.0) {
        r = 1.0;
        g = -1.0 * (wave - 645.0) / (645.0 - 580.0);
    } else if (wave >= 645.0 && wave <= 780.0) {
        r = 1.0;
    }

    double s = 1.0;
    if (wave > 700.0)
        s = 0.3 + 0.7 * (780.0 - wave) / (780.0 - 700.0);
    else if (wave <  420.0)
        s = 0.3 + 0.7 * (wave - 380.0) / (420.0 - 380.0);

    r = std::pow(r * s, 0.8);
    g = std::pow(g * s, 0.8);
    b = std::pow(b * s, 0.8);
    return qRgb(int(r * 255), int(g * 255), int(b * 255));
}

int MandelbrotRenderer::tileBegin(int tileIndex, int totalSize)
{
    int tileSize = (totalSize / (tileCount -1));
    return tileIndex * tileSize;
}

int MandelbrotRenderer::tileEnd(int tileIndex, int totalSize)
{
    int tileSize = (totalSize / (tileCount -1));
    return (tileIndex * tileSize + tileSize) % (totalSize);
}

RenderThread::RenderThread(int threadCount, QObject *parent)
    : QThread(parent)
{
    restart = false;
    abort = false;
    
    int threadAndTileCount = threadCount;

    renderer = new MandelbrotRenderer(
        threadAndTileCount * 2,
        threadAndTileCount,
        [this]()-> bool {
            QMutexLocker locker(&mutex);
            return abort || restart;
        },
        [this](const QImage &image, double scaleFactor) {
            qDebug() << "emit renderedImage";
            emit renderedImage(image.copy(), scaleFactor);
        }    
    );
}

RenderThread::~RenderThread()
{
    mutex.lock();
    abort = true;
    condition.wakeOne();
    mutex.unlock();

    wait();
    
    delete renderer;
}

void RenderThread::render(double centerX, double centerY, double scaleFactor,
    QSize resultSize, double resultDevicePixelRatio)
{
    QMutexLocker locker(&mutex);

    this->centerX = centerX;
    this->centerY = centerY;
    this->scaleFactor = scaleFactor;
    this->resultSize = resultSize;
    this->resultDevicePixelRatio = resultDevicePixelRatio;

    if (!isRunning()) {
        start(LowPriority);
    } else {
        restart = true;
        condition.wakeOne();
    }
}
void RenderThread::run()
{
    forever {
        mutex.lock();
        QSize resultSize = this->resultSize;
        double scaleFactor = this->scaleFactor;
        double centerX = this->centerX;
        double centerY = this->centerY;
        double resultDevicePixelRatio = this->resultDevicePixelRatio;
        mutex.unlock();

        renderer->render(centerX, centerY, scaleFactor, resultSize, resultDevicePixelRatio);

        {
            QMutexLocker locker(&mutex);            
            if (abort)
                return;
            if (!restart)
                condition.wait(&mutex);
            restart = false;
        }
    }
}
