var g_qtimerdebug_realSetTimeout = undefined;
var g_qtimerdebug_realSetInterval = undefined;
var g_qtimerdebug_realClearTimeout = undefined;
var g_qtimerdebug_realClearInterval = undefined;

var g_qtimerdebug_isRunning = true;
var g_qtimerdebug_timerTicks = 0;
var g_qtimerdebug_currentTimerId = -1;
var g_qtimerdebug_activeTimeOuts = []; // array of { id, milliseconds, targetTime, func }
var g_qtimerdebug_activeTimersChangedCallback = undefined;

function qtimerdebug_interpose()
{
    g_qtimerdebug_realSetTimeout = window.setTimeout;
    window.setTimeout = qtimerdebug_setTimeout;
    g_qtimerdebug_realSetInterval = window.setInterval;
    window.setInterval = qtimerdebug_setInterval;
    g_qtimerdebug_realClearTimeout  = window.clearTimeout ;
    window.clearTimeout = qtimerdebug_clearTimeout;
    g_qtimerdebug_realClearInterval = window.clearInterval;
    window.clearInterval = qtimerdebug_clearInterval;
}

function qtimerdebug_fireNextTimer()
{
    let timer = g_qtimerdebug_activeTimeOuts.shift();
    if (timer === undefined)
        return;
    timer.func();
    ++g_qtimerdebug_timerTicks;
}

function qtimerdebug_fireAllTimers()
{
    let timerList = g_qtimerdebug_activeTimeOuts; // fire currently registered timers only.
    g_qtimerdebug_activeTimeOuts = [];
    while (timerList.length > 0) {
        let timer = timerList.shift();
        timer.func();
        ++g_qtimerdebug_timerTicks;
    }
}

function qtimerdebug_fireElapsedTimers()
{
    let now = Date.now();
    let fire = g_qtimerdebug_activeTimeOuts.filter(timer => timer.targetTime <= now);
    let keep = g_qtimerdebug_activeTimeOuts.filter(timer => timer.targetTime > now);
    g_qtimerdebug_activeTimeOuts = keep;
    fire.map(timer => {
        timer.func();
        ++g_qtimerdebug_timerTicks;
    });
}

function qtimerdebug_nextTimerId()
{
    ++g_qtimerdebug_currentTimerId;
    return g_qtimerdebug_currentTimerId;
}

function qtimerdebug_setRunning(running)
{
    g_qtimerdebug_isRunning = running;
    if (running) {
        qtimerdebug_fireElapsedTimers();
        qtimerdebug_updateActiveTimersChangedCallback();
    }
}

function qtimerdebug_step()
{
    qtimerdebug_fireNextTimer();
    qtimerdebug_updateActiveTimersChangedCallback();
}

function qtimerdebug_stepAll()
{
    qtimerdebug_fireAllTimers();
    qtimerdebug_updateActiveTimersChangedCallback();
}

function qtimerdebug_setActiveTimersChangedCallback(callback)
{
    g_qtimerdebug_activeTimersChangedCallback = callback;
}

function qtimerdebug_updateActiveTimersChangedCallback()
{
    if (g_qtimerdebug_activeTimersChangedCallback === undefined)
        return;
    let timeouts = g_qtimerdebug_activeTimeOuts.map(timer => timer.milliseconds);
    let intervals = [];
    g_qtimerdebug_activeTimersChangedCallback(g_qtimerdebug_timerTicks, timeouts, intervals);
}

function qtimerdebug_setTimeout(func, milliseconds)
{
    let timer = {
        id: qtimerdebug_nextTimerId(),
        milliseconds:milliseconds,
        targetTime: Date.now() + milliseconds,
        func: func
    }
    g_qtimerdebug_activeTimeOuts.push(timer);
    g_qtimerdebug_activeTimeOuts.sort((a, b) => a.targetTime > b.targetTime);

    if (g_qtimerdebug_isRunning)
        g_qtimerdebug_realSetTimeout(() => {
            qtimerdebug_fireElapsedTimers();
            qtimerdebug_updateActiveTimersChangedCallback();
        }, milliseconds);

    qtimerdebug_updateActiveTimersChangedCallback();
    return timer.id;
}

function qtimerdebug_clearTimeout(timerId)
{
    g_qtimerdebug_activeTimeOuts = g_qtimerdebug_activeTimeOuts.filter(timer => timer.id != timerId);
    qtimerdebug_updateActiveTimersChangedCallback();
}

function qtimerdebug_setInterval(func, milliseconds)
{
    let handle = g_qtimerdebug_realSetInterval(func, milliseconds);
    // console.log("setInterval " + milliseconds + " -> " + handle);

    return handle;
}

function qtimerdebug_clearInterval(handle)
{
    // console.log("clearInterval " + handle);
    g_qtimerdebug_realClearInterval(handle);
}


