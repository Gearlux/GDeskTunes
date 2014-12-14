#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <QStateMachine>
#include <QState>

class State: public QState
{
    Q_OBJECT

public:
    State(QState *parent = 0);
    State(ChildMode childMode, QState *parent = 0);

    void onEntry(QEvent *event);
};

class StateMachine : public QStateMachine
{
    Q_OBJECT

public:
    StateMachine();
    ~StateMachine();

};

#endif // STATEMACHINE_H
