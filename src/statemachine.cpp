#include "statemachine.h"

#include <QDebug>

State::State(QState *parent) :
    QState(parent)
{}

State::State(ChildMode childMode, QState *parent) :
    QState(childMode, parent)
{}

void State::onEntry(QEvent *)
{
    qDebug() << "Entering state: " << objectName();
}

StateMachine::StateMachine()
{

}

StateMachine::~StateMachine()
{

}


