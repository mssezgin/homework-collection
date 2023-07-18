#include "hw2_output.h"

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <setjmp.h>
#include <sys/time.h>
#include <time.h>
#include <vector>


pthread_mutex_t globalMutex;
pthread_cond_t commandersTurn;
pthread_cond_t othersTurn;


///// types /////

// common

struct tuple {
    size_t i, j;                    // i: height, j: width
};

// grid

enum user_t {
    PP_GATHER,
    SS_SIT,
    SS_THROW
};

class cell_t {
private:
    size_t nOfCigbutts;             // number of cigbutts in the cell
    pthread_mutex_t mutexCigbutts;  // mutex used while reading/writing the nOfCigbutts
    bool ppGathering;               // whether a proper private has locked the cell and is gathering cigbutts inside
    size_t nOfPPsWaitingToGather;   // number of proper privaes who are waiting to lock the cell and start gathering cigbutts
    bool ssSitting;                 // whether a sneaky smoker has locked the cell and is smoking inside
    size_t nOfSSsWaitingToSit;      // number of sneaky smokers who are waiting to lock the cell and start sitting
    size_t nOfSSsThrowing;          // number of sneaky smokers who can reach the cell to throw cigbutts (sitting sneaky smokers excluded)
    size_t nOfSSsWaitingToThrow;    // number of sneaky smokers who are waiting to lock the cell and start throwing cigbutts
    pthread_cond_t ppCanGather;     // conditional variable on which proper privates wait to lock the cell to gather cigbutts
    pthread_cond_t ssCanSit;        // conditional variable on which sneaky smokers wait to lock the cell to sit
    pthread_cond_t ssCanThrow;      // conditional variable on which sneaky smokers wait to lock the cell to throw cigbutts
    pthread_mutex_t mutexState;     // mutex used while changing who is inside or is waiting outside

    bool tryToLockForPPToGather() {
        bool b;
        if (ppGathering == true || ssSitting == true || nOfSSsThrowing > 0) {
            b = false;
        } else {
            ppGathering = true;
            b = true;
        }
        return b;
    }

    void waitForASignalForPPToGather() {
        nOfPPsWaitingToGather++;
        pthread_cond_wait(&ppCanGather, &mutexState);
        nOfPPsWaitingToGather--;
    }

    void unlockSincePPFinishedGathering() {
        ppGathering = false;
        if (nOfPPsWaitingToGather > 0) {
            pthread_cond_signal(&ppCanGather);
        }
        if (nOfSSsWaitingToSit > 0) {
            pthread_cond_signal(&ssCanSit);
        }
        if (nOfSSsWaitingToThrow > 0) {
            pthread_cond_broadcast(&ssCanThrow);
        }
    }

    bool tryToLockForSSToSit() {
        bool b;
        if (ppGathering == true || ssSitting == true) {
            b = false;
        } else {
            ssSitting = true;
            b = true;
        }
        return b;
    }

    void waitForASignalForSSToSit() {
        nOfSSsWaitingToSit++;
        pthread_cond_wait(&ssCanSit, &mutexState);
        nOfSSsWaitingToSit--;
    }

    void unlockSinceSSFinishedSitting() {
        ssSitting = false;
        if (nOfPPsWaitingToGather > 0) {
            pthread_cond_signal(&ppCanGather);
        }
        if (nOfSSsWaitingToSit > 0) {
            pthread_cond_signal(&ssCanSit);
        }
        // should not happen.
        // if (nOfSSsWaitingToThrow > 0) {
        //     pthread_cond_broadcast(&ssCanThrow);
        // }
    }

    bool tryToLockForSSToThrow() {
        bool b;
        if (ppGathering == true) {
            b = false;
        } else {
            nOfSSsThrowing++;
            b = true;
        }
        return b;
    }

    void waitForASignalForSSToThrow() {
        nOfSSsWaitingToThrow++;
        pthread_cond_wait(&ssCanThrow, &mutexState);
        nOfSSsWaitingToThrow--;
    }

    void unlockSinceSSFinishedThrowing() {
        nOfSSsThrowing--;
        if (nOfPPsWaitingToGather > 0) {
            pthread_cond_signal(&ppCanGather);
        }
        // should not happen.
        // if (nOfSSsWaitingToSit > 0) {
        //     pthread_cond_signal(&ssCanSit);
        // }
        // should not happen.
        // if (nOfSSsWaitingToThrow > 0) {
        //     pthread_cond_broadcast(&ssCanThrow);
        // }
    }

public:
    cell_t(size_t _nOfCigbutts = 0) :
        nOfCigbutts(_nOfCigbutts),
        ppGathering(false),
        nOfPPsWaitingToGather(0),
        ssSitting(false),
        nOfSSsWaitingToSit(0),
        nOfSSsThrowing(0),
        nOfSSsWaitingToThrow(0) {

        pthread_mutex_init(&mutexCigbutts, NULL);
        pthread_mutex_init(&mutexState, NULL);
        pthread_cond_init(&ppCanGather, NULL);
        pthread_cond_init(&ssCanSit, NULL);
        pthread_cond_init(&ssCanThrow, NULL);
    }

    ~cell_t() {
        pthread_mutex_destroy(&mutexCigbutts);
        pthread_mutex_destroy(&mutexState);
        pthread_cond_destroy(&ppCanGather);
        pthread_cond_destroy(&ssCanSit);
        pthread_cond_destroy(&ssCanThrow);
    }

    size_t getNOfCigbutts() {
        pthread_mutex_lock(&mutexCigbutts);
        size_t n = nOfCigbutts;
        pthread_mutex_unlock(&mutexCigbutts);
        return n;
    }

    void incrementNOfCigbutts() {
        pthread_mutex_lock(&mutexCigbutts);
        nOfCigbutts++;
        pthread_mutex_unlock(&mutexCigbutts);
    }

    void decrementNOfCigbutts() {
        pthread_mutex_lock(&mutexCigbutts);
        nOfCigbutts--;
        pthread_mutex_unlock(&mutexCigbutts);
    }

    /**
     * If the cell can be used by the caller, this function locks the cell.
     * 
     * @param callerType Specifies who is trying to lock the cell for what
     * purpose. Should be PP_GATHER, SS_SIT, or SS_THROW.
     * 
     * @return true if the lock is successful, false otherwise.
     */
    bool tryToLock(user_t callerType) {
        bool b;
        pthread_mutex_lock(&mutexState);
        switch (callerType) {
            case PP_GATHER:
                b = tryToLockForPPToGather();
                break;
            case SS_SIT:
                b = tryToLockForSSToSit();
                break;
            case SS_THROW:
                b = tryToLockForSSToThrow();
                break;
            default:
                b = false;
                break;
        }
        pthread_mutex_unlock(&mutexState);
        return b;
    }

    /**
     * Waits until the cell becomes able to be used by the caller. Does not
     * lock the cell when the signal comes. Therefore, user should 'tryToLock'
     * again.
     * 
     * @param callerType Specifies who is waiting for a signal to lock the cell
     * for what purpose. Should be PP_GATHER, SS_SIT, or SS_THROW.
     */
    void waitForASignal(user_t callerType) {
        pthread_mutex_lock(&mutexState);
        switch (callerType) {
            case PP_GATHER:
                waitForASignalForPPToGather();
                break;
            case SS_SIT:
                waitForASignalForSSToSit();
                break;
            case SS_THROW:
                waitForASignalForSSToThrow();
                break;
            default:
                break;
        }
        pthread_mutex_unlock(&mutexState);
    }

    /**
     * If the cell can be used by the caller, this function returns imediately.
     * Otherwise, waits until the cell becomes able to be used by the caller.
     * Does not lock the cell when the signal comes. Therefore, user should
     * 'tryToLock' again.
     * 
     * @param callerType Specifies who is trying to lock the cell for what
     * purpose. Should be PP_GATHER, SS_SIT, or SS_THROW.
     */
    void tryToLockAndWaitForASignal(user_t callerType) {
        pthread_mutex_lock(&mutexState);
        switch (callerType) {
            case PP_GATHER:
                if (tryToLockForPPToGather() == true) {
                    unlockSincePPFinishedGathering();
                } else {
                    waitForASignalForPPToGather();
                }
                break;
            case SS_SIT:
                if (tryToLockForSSToSit() == true) {
                    unlockSinceSSFinishedSitting();
                } else {
                    waitForASignalForSSToSit();
                }
                break;
            case SS_THROW:
                if (tryToLockForSSToThrow() == true) {
                    unlockSinceSSFinishedThrowing();
                } else {
                    waitForASignalForSSToThrow();
                }
                break;
            default:
                break;
        }
        pthread_mutex_unlock(&mutexState);
    }

    /**
     * Unlocks the cell. Signals all users who 'waitForASignal' without
     * checking their condition. Therefore, there is no priority among waiters. 
     * 
     * @param callerType Specifies who is unlocking the cell since they
     * finished doing what. Should be PP_GATHER, SS_SIT, or SS_THROW.
     */
    void unlock(user_t callerType) {
        pthread_mutex_lock(&mutexState);
        switch (callerType) {
            case PP_GATHER:
                unlockSincePPFinishedGathering();
                break;
            case SS_SIT:
                unlockSinceSSFinishedSitting();
                break;
            case SS_THROW:
                unlockSinceSSFinishedThrowing();
                break;
            default:
                break;
        }
        pthread_mutex_unlock(&mutexState);
    }
};

// proper private

struct proper_private_t {
    size_t id;                      // proper private id
    tuple s;                        // dimensions of areas
    time_t tg;                      // time interval (in msec) between picking cigbutts
    std::vector<tuple> coords;      // list of top-left coordinates of areas
    jmp_buf checkpoint;
};

// commander

enum order_type {
    NO_ORDER,                  // unknown order
    BREAK_ORDER,                    // break order
    CONTINUE_ORDER,                 // continue order
    STOP_ORDER                      // stop order
};

struct order_t {
    time_t time;                    // order time
    order_type type;                // order type
};

struct commander_t {
    std::vector<order_t> orders;    // list of orders
    order_type newOrderType;        // type of the new order
    timespec newOrderTime;          // given time of the new order
    pthread_mutex_t mutexOrder;     // mutex for the conditional variable 'isThereAnOrder'
    pthread_cond_t isThereAnOrder;  // conditional variable through which the commander broadcasts orders to everybody
    bool isWaiting;
};

// sneaky smoker

struct smoking {
    tuple center;                   // center coordinate of smoking
    size_t n;                       // number of cigarette butts to throw
};

struct sneaky_smoker_t {
    size_t id;                      // sneaky smoker id
    time_t ts;                      // time interval (in msec) between smokings
    std::vector<smoking> smokings;  // list of smokings
};


///// global variables /////

pthread_t *threads;
tuple gridDimensions;
std::vector<std::vector<cell_t>> grid;
std::vector<proper_private_t> properPrivates;
commander_t commander;
std::vector<sneaky_smoker_t> sneakySmokers;


///// functions /////

// common

timespec addMsecToTimespec(const timespec &ts, const time_t &tmsec) {
    timespec newts;
    long l = ts.tv_nsec + (tmsec % 1000L) * 1000000L;
    newts.tv_nsec = l % 1000000000L;
    newts.tv_sec = ts.tv_sec + tmsec / 1000L + l / 1000000000L;
    return newts;
}

timespec msecLaterFromNow(const time_t &tmsec) {
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    /* long l = ts.tv_nsec + (tmsec % 1000L) * 1000000L;
    ts.tv_nsec = l % 1000000000L;
    ts.tv_sec += (tmsec / 1000L + l / 1000000000L); */
    ts = addMsecToTimespec(ts, tmsec);
    return ts;
}

void lockAll(user_t callerType, tuple topLeft, tuple bottomRight) {

    bool lr;                    // lock result, false if any cell could not be locked
    tuple cptr;                 // used as a pointer to a cell

    while (true) {
        lr = true;

        // try to lock every cell in the current area.
        // if a cell cannot be locked, then lr becomes false, loops break, and
        // cptr points to that cell.
        for (size_t i = topLeft.i; i <= bottomRight.i; i++) {
            for (size_t j = topLeft.j; j <= bottomRight.j; j++) {
                lr = grid[i][j].tryToLock(callerType);
                if (lr == false) {
                    cptr.j = j;
                    break;
                }
            }
            if (lr == false) {
                cptr.i = i;
                break;
            }
        }

        // if every cell has been locked successfully, break the loop.
        if (lr == true) {
            break;
        }

        // if not, unlock all locked cells.
        // first, unlock all the cells in each row until the last row, which is
        // the row of the cell pointed by cptr.
        for (size_t i = topLeft.i; i < cptr.i; i++) {
            for (size_t j = topLeft.j; j <= bottomRight.j; j++) {
                grid[i][j].unlock(callerType);
            }
        }
        // second, unlock each cell in the last row until the cell pointed by cptr.
        for (size_t j = topLeft.j; j < cptr.j; j++) {
            grid[cptr.i][j].unlock(callerType);
        }

        // try again once more and wait for the cell that could not be locked to become available.
        grid[cptr.i][cptr.j].tryToLockAndWaitForASignal(callerType);
    }
}

void unlockAll(user_t callerType, tuple topLeft, tuple bottomRight) {
    for (size_t i = topLeft.i; i <= bottomRight.i; i++) {
        for (size_t j = topLeft.j; j <= bottomRight.j; j++) {
            grid[i][j].unlock(callerType);
        }
    }
}

// proper private

void *properPrivateThreadFunction(void *ppPtr) {

    // proper private
    proper_private_t &pp = *(proper_private_t *)ppPtr;

    /**********************************************/
    hw2_notify(PROPER_PRIVATE_CREATED, pp.id, 0, 0);
    /**********************************************/

    // for each gathering area in the list of gathering areas of pp
    // for each (  topLeft   ) in (           pp.coords           )
    for (auto topLeft = pp.coords.begin(); topLeft != pp.coords.end(); ++topLeft) {

        order_type lastOrderType = NO_ORDER;
        timespec lastOrderTime;
        lastOrderTime.tv_sec = 0;
        lastOrderTime.tv_nsec = 0;

        tuple bottomRight;
        bottomRight.i = topLeft->i + pp.s.i - 1;
        bottomRight.j = topLeft->j + pp.s.j - 1;

        // set checkpoint.
        if (setjmp(pp.checkpoint) != 0) {

            // came here because of break order. wait for a continue order.
            pthread_mutex_lock(&globalMutex);
            // unlock all the cells in the area.
            unlockAll(PP_GATHER, *topLeft, bottomRight);
            lastOrderType = BREAK_ORDER;
            lastOrderTime = commander.newOrderTime;
            while (lastOrderType != CONTINUE_ORDER && lastOrderType != STOP_ORDER) {
                pthread_cond_wait(&othersTurn, &globalMutex);
                if (commander.newOrderTime.tv_sec != lastOrderTime.tv_sec || commander.newOrderTime.tv_nsec != lastOrderTime.tv_nsec) {
                    // there is a new order.
                    lastOrderType = commander.newOrderType;
                    lastOrderTime = commander.newOrderTime;
                }
            }
            pthread_mutex_unlock(&globalMutex);

            // exit if the stop order was given.
            if (lastOrderType == STOP_ORDER) {
                /**********************************************/
                hw2_notify(PROPER_PRIVATE_STOPPED, pp.id, 0, 0);
                /**********************************************/
                return NULL;
            }
        }

        // lock all the cells in the area.
        lockAll(PP_GATHER, *topLeft, bottomRight);

        /****************************************************************/
        hw2_notify(PROPER_PRIVATE_ARRIVED, pp.id, topLeft->i, topLeft->j);
        /****************************************************************/

        // gather cigbutts from all the cells in the area.
        pthread_mutex_lock(&globalMutex);
        for (size_t i = topLeft->i; i <= bottomRight.i; i++) {
            for (size_t j = topLeft->j; j <= bottomRight.j; j++) {
                while (grid[i][j].getNOfCigbutts() > 0) {

                    // calculate wake up time.
                    timespec until = msecLaterFromNow(pp.tg);

                    // give turn to commander if they are waiting.
                    if (commander.isWaiting == true) {
                        pthread_cond_signal(&commandersTurn);
                    }

                    while (true) {
                        // sleep until tg miliseconds later.
                        int rc = pthread_cond_timedwait(&othersTurn, &globalMutex, &until);

                        if (rc == ETIMEDOUT) {
                            // there is no signal. tg miliseconds has passed. gather a cigbutt.
                            grid[i][j].decrementNOfCigbutts();
                            /***********************************************/
                            hw2_notify(PROPER_PRIVATE_GATHERED, pp.id, i, j);
                            /***********************************************/
                            break;
                        }

                        // there is a signal. check for a new order.
                        if (commander.newOrderType != NO_ORDER) {
                            switch (commander.newOrderType) {
                                case BREAK_ORDER:
                                    longjmp(pp.checkpoint, 1);
                                    break;
                                case CONTINUE_ORDER:
                                    // do nothing.
                                    break;
                                case STOP_ORDER:
                                    // unlock all the cells in the area.
                                    unlockAll(PP_GATHER, *topLeft, bottomRight);
                                    // exit.
                                    /**********************************************/
                                    hw2_notify(PROPER_PRIVATE_STOPPED, pp.id, 0, 0);
                                    /**********************************************/
                                    pthread_mutex_unlock(&globalMutex);
                                    return NULL;
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                }
            }
        }
        pthread_mutex_unlock(&globalMutex);

        /**********************************************/
        hw2_notify(PROPER_PRIVATE_CLEARED, pp.id, 0, 0);
        /**********************************************/

        // unlock all the cells in the area.
        unlockAll(PP_GATHER, *topLeft, bottomRight);
    }
    
    /*********************************************/
    hw2_notify(PROPER_PRIVATE_EXITED, pp.id, 0, 0);
    /*********************************************/

    return NULL;
}

// commander

void *commanderThreadFunction(void *) {

    timespec startTime;
    clock_gettime(CLOCK_REALTIME, &startTime);

    pthread_mutex_lock(&globalMutex);
    commander.isWaiting = false;
    for (auto order = commander.orders.begin(); order != commander.orders.end(); ++order) {
        timespec until = addMsecToTimespec(startTime, order->time);
        // sleep
        // sleep(order->time);
        // while (pthread_cond_timedwait(&commandersTurn, &globalMutex, &until) != ETIMEDOUT);
        pthread_cond_timedwait(&commandersTurn, &globalMutex, &until);

        // wait
        commander.isWaiting = true;
        pthread_cond_wait(&commandersTurn, &globalMutex);
        commander.isWaiting = false;

        // give the order
        commander.newOrderType = order->type;
        commander.newOrderTime =until;
        switch (commander.newOrderType) {
            case NO_ORDER:
                break;
            case BREAK_ORDER:
                /*************************************************/
                /******/ hw2_notify(ORDER_BREAK, 0, 0, 0); /******/
                /*************************************************/
                break;
            case CONTINUE_ORDER:
                /****************************************************/
                /******/ hw2_notify(ORDER_CONTINUE, 0, 0, 0); /******/
                /****************************************************/
                break;
            case STOP_ORDER:
                /************************************************/
                /******/ hw2_notify(ORDER_STOP, 0, 0, 0); /******/
                /************************************************/
                break;
            default:
                break;
        }
        pthread_cond_broadcast(&othersTurn);
    }
    pthread_mutex_unlock(&globalMutex);

    return NULL;
}

// sneaky smoker

void *sneakySmokerThreadFunction(void *ssPtr) {

    // sneaky smoker
    sneaky_smoker_t &ss = *(sneaky_smoker_t *)ssPtr;

    /*********************************************/
    hw2_notify(SNEAKY_SMOKER_CREATED, ss.id, 0, 0);
    /*********************************************/

    // for each smoking area in the list of smoking areas of ss
    // for each (   cntr   ) in (         ss.smoking          )
    for (auto smoking = ss.smokings.begin(); smoking != ss.smokings.end(); ++smoking) {

        tuple around[8] = {
            {smoking->center.i - 1, smoking->center.j - 1},
            {smoking->center.i - 1, smoking->center.j    },
            {smoking->center.i - 1, smoking->center.j + 1},
            {smoking->center.i    , smoking->center.j + 1},
            {smoking->center.i + 1, smoking->center.j + 1},
            {smoking->center.i + 1, smoking->center.j    },
            {smoking->center.i + 1, smoking->center.j - 1},
            {smoking->center.i    , smoking->center.j - 1}
        };
        tuple &topLeft = around[0];
        tuple &bottomRight = around[4];

        // lock all the cells in the area.
        while (true) {
            lockAll(SS_THROW, topLeft, bottomRight);
            if (grid[smoking->center.i][smoking->center.j].tryToLock(SS_SIT) == true) {
                break;
            }
            unlockAll(SS_THROW, topLeft, bottomRight);
            grid[smoking->center.i][smoking->center.j].tryToLockAndWaitForASignal(SS_SIT);
        }

        /*****************************************************************************/
        hw2_notify(SNEAKY_SMOKER_ARRIVED, ss.id, smoking->center.i, smoking->center.j);
        /*****************************************************************************/

        // smoke and throw cigbutts to the around.
        pthread_mutex_lock(&commander.mutexOrder);
        for (size_t n = smoking->n, k = 0; n > 0; n--, k = (k + 1) % 8) {

            // sleep for ts miliseconds. wake up in case of a stop order.
            timespec until = msecLaterFromNow(ss.ts);
            int rc = pthread_cond_timedwait(&commander.isThereAnOrder, &commander.mutexOrder, &until);
            if (rc == 0) {
                // there is a new order. carry out it.
                switch (commander.newOrderType) {
                    case NO_ORDER:
                        // do nothing
                        break;
                    case BREAK_ORDER:
                        // do nothing
                        break;
                    case CONTINUE_ORDER:
                        // do nothing
                        break;
                    case STOP_ORDER:
                        // TODO:
                        break;
                    default:
                        break;
                }
            } else if (rc == ETIMEDOUT) {
                // there is no order. gather a cigbutt.
                grid[around[k].i][around[k].j].incrementNOfCigbutts();
                /*****************************************************************/
                hw2_notify(SNEAKY_SMOKER_FLICKED, ss.id, around[k].i, around[k].j);
                /*****************************************************************/
            }
        }
        pthread_mutex_unlock(&commander.mutexOrder);

        /******************************************/
        hw2_notify(SNEAKY_SMOKER_LEFT, ss.id, 0, 0);
        /******************************************/

        // unlock all the cells in the area.
        grid[smoking->center.i][smoking->center.j].unlock(SS_SIT);
        unlockAll(SS_THROW, topLeft, bottomRight);
    }

    /********************************************/
    hw2_notify(SNEAKY_SMOKER_EXITED, ss.id, 0, 0);
    /********************************************/

    return NULL;
}

// main

void initEverything() {

    // mutex and conditional variables
    pthread_mutex_init(&globalMutex, NULL);
    pthread_cond_init(&commandersTurn, NULL);
    pthread_cond_init(&othersTurn, NULL);

    size_t n, m;
    std::string s;

    // grid
    std::cin >> gridDimensions.i >> gridDimensions.j;
    for (size_t i = 0; i < gridDimensions.i; i++) {
        std::vector<cell_t> currentRow;
        for (size_t j = 0; j < gridDimensions.j; j++) {
            std::cin >> n;
            currentRow.push_back(cell_t(n));
        }
        grid.push_back(currentRow);
    }

    // proper privates
    std::cin >> n;
    for (size_t i = 0; i < n; i++) {
        proper_private_t currentPP;
        std::cin >> currentPP.id >> currentPP.s.i >> currentPP.s.j >> currentPP.tg;
        std::cin >> m;
        for (size_t j = 0; j < m; j++) {
            tuple coord;
            std::cin >> coord.i >> coord.j;
            currentPP.coords.push_back(coord);
        }
        properPrivates.push_back(currentPP);
    }

    // check eof
    if (std::cin.eof()) {
        return;
    }

    // commander orders
    std::cin >> n;
    for (size_t i = 0; i < n; i++) {
        order_t currentOrder;
        std::cin >> currentOrder.time;
        std::cin >> s;
        if (s == "break") {
            currentOrder.type = BREAK_ORDER;
        } else if (s == "continue") {
            currentOrder.type = CONTINUE_ORDER;
        } else if (s == "stop") {
            currentOrder.type = STOP_ORDER;
        } else {
            currentOrder.type = NO_ORDER;
        }
        commander.orders.push_back(currentOrder);
    }

    // check eof
    if (std::cin.eof()) {
        return;
    }

    // sneaky smokers
    std::cin >> n;
    for (size_t i = 0; i < n; i++) {
        sneaky_smoker_t currentSS;
        std::cin >> currentSS.id >> currentSS.ts;
        std::cin >> m;
        for (size_t j = 0; j < m; j++) {
            smoking currentSmoking;
            std::cin >> currentSmoking.center.i >> currentSmoking.center.j >> currentSmoking.n;
            currentSS.smokings.push_back(currentSmoking);
        }
        sneakySmokers.push_back(currentSS);
    }
}

void checkInput() {
    std::cout << "grid: " << gridDimensions.i << " " << gridDimensions.j << std::endl;
    for (auto itr = grid.begin(); itr != grid.end(); ++itr) {
        for (auto itr2 = itr->begin(); itr2 != itr->end(); ++itr2) {
            std::cout << itr2->getNOfCigbutts() << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "properPrivates: " << properPrivates.size() << std::endl;
    for (auto itr = properPrivates.begin(); itr != properPrivates.end(); ++itr) {
        std::cout << "gid: " << itr->id << " si: " << itr->s.i << " sj: " << itr->s.j << " tg: " << itr->tg << " ng: " << itr->coords.size() << std::endl;
        for (auto itr2 = itr->coords.begin(); itr2 != itr->coords.end(); ++itr2) {
            std::cout << itr2->i << " " << itr2->j << std::endl;
        }
    }
    std::cout << "commander: " << commander.orders.size() << std::endl;
    for (auto itr = commander.orders.begin(); itr != commander.orders.end(); ++itr) {
        std::cout << itr->type << " " << itr->time << std::endl;
    }
    std::cout << "sneakySmokers: " << sneakySmokers.size() << std::endl;
    for (auto itr = sneakySmokers.begin(); itr != sneakySmokers.end(); ++itr) {
        std::cout << "sid: " << itr->id << " ts: " << itr->ts << " ng: " << itr->smokings.size() << std::endl;
        for (auto itr2 = itr->smokings.begin(); itr2 != itr->smokings.end(); ++itr2) {
            std::cout << itr2->center.i << " " << itr2->center.j << " " << itr2->n << std::endl;
        }
    }
}

void destroyEverything() {

    // mutex and conditional variables
    pthread_mutex_destroy(&globalMutex);
    pthread_cond_destroy(&commandersTurn);
    pthread_cond_destroy(&othersTurn);

    // threads
    delete[] threads;
}

void createThreads() {

    size_t i = 0;
    threads = new pthread_t[properPrivates.size() + 1 + sneakySmokers.size()];

    // proper privates
    for (auto itr = properPrivates.begin(); itr != properPrivates.end(); ++itr) {
        pthread_create(&threads[i++], NULL, properPrivateThreadFunction, (void *)&*itr);
    }

    // commander
    pthread_create(&threads[i++], NULL, commanderThreadFunction, NULL);

    // sneaky smokers
    for (auto itr = sneakySmokers.begin(); itr != sneakySmokers.end(); ++itr) {
        pthread_create(&threads[i++], NULL, sneakySmokerThreadFunction, (void *)&*itr);
    }
}

void joinThreads() {
    size_t n = properPrivates.size() + 1 + sneakySmokers.size();
    for (size_t i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }
}

int main() {

    // start
    initEverything();
    // checkInput();

    // init notifier
    hw2_init_notifier();

    // create threads
    createThreads();

    // join threads
    joinThreads();

    // finish
    destroyEverything();

    // exit
    exit(EXIT_SUCCESS);
}
