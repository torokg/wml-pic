#include <set>
#include <mutex>
#include "process.hh"

namespace sched
{
    
static std::set<process*> _proc;

static std::set<process*>::iterator _current = _proc.end();


void 
process::init()
{}

void
process::uninit()
{}


const std::string&
process::name()
{
    static std::string n("sched::process");
    return n;
}

process::~process()
{ throw std::logic_error("you cannot delete a process yet"); }


process::process()
{}

void
process::start()
{
    if(_current != _proc.end() && *_current == this)
        return;
    if(_proc.emplace(this).second)
        _state = 0;
}

void
process::stop()
{ _state = 3; }

void
process::notify()
{ _notified = 1; }

bool
process::suspended() const
{ return _state == 2; }

void
process::suspend()
{ _state = 2; }
	
bool
process::notified() const
{ return _notified; }

bool
process::is_current() const
{ return *_current == this; }

void
process::_serve()
{
    _current = _proc.begin();
    while(_current != _proc.end())
    {
        switch((*_current)->_state)
        {
            case 0:     // Started
                (*_current)->init();
                if((*_current)->_state == 0)
                    (*_current)->_state = 1;
                break;
                
            case 1:     // Running
                (*_current)->exec();
                break;
                  
            case 2:     // Suspended
                if((*_current)->_notified)
                {
                    (*_current)->_state = 1;
                    (*_current)->_notified = 0;
                }
                
                // TODO: check for suspension timeout
                break;
                
            case 3:     // Stopped
                (*_current)->uninit();
                _current = _proc.erase(_current);
                continue;
        }
        ++_current;
    }
}

}
