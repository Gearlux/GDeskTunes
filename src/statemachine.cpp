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
    QState* parent = dynamic_cast<QState*>( parentState() );
    QString name = objectName();
    while ( parent != 0 )
    {
        name = parent->objectName() + "->" + name;
        parent = dynamic_cast<QState*>( parent->parentState() );
    }
    qDebug() << "Entering state: " << name;
}

StateMachine::StateMachine()
{

}

StateMachine::~StateMachine()
{

}


