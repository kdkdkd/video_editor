/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-10 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#ifndef __JUCE_MOUSEEVENT_JUCEHEADER__
#define __JUCE_MOUSEEVENT_JUCEHEADER__

class Component;
class MouseInputSource;
#include "../keyboard/juce_ModifierKeys.h"
#include "../../../core/juce_Time.h"
#include "../../graphics/geometry/juce_Point.h"


//==============================================================================
/**
    Contains position and status information about a mouse event.

    @see MouseListener, Component::mouseMove, Component::mouseEnter, Component::mouseExit,
         Component::mouseDown, Component::mouseUp, Component::mouseDrag
*/
class JUCE_API  MouseEvent
{
public:
    //==============================================================================
    /** Creates a MouseEvent.

        Normally an application will never need to use this.

        @param source           the source that's invoking the event
        @param position         the position of the mouse, relative to the component that is passed-in
        @param modifiers        the key modifiers at the time of the event
        @param eventComponent   the component that the mouse event applies to
        @param originator       the component that originally received the event
        @param eventTime        the time the event happened
        @param mouseDownPos     the position of the corresponding mouse-down event (relative to the component that is passed-in).
                                If there isn't a corresponding mouse-down (e.g. for a mouse-move), this will just be
                                the same as the current mouse-x position.
        @param mouseDownTime    the time at which the corresponding mouse-down event happened
                                If there isn't a corresponding mouse-down (e.g. for a mouse-move), this will just be
                                the same as the current mouse-event time.
        @param numberOfClicks   how many clicks, e.g. a double-click event will be 2, a triple-click will be 3, etc
        @param mouseWasDragged  whether the mouse has been dragged significantly since the previous mouse-down
    */
    MouseEvent (MouseInputSource& source,
                const Point<int>& position,
                const ModifierKeys& modifiers,
                Component* eventComponent,
                Component* originator,
                const Time& eventTime,
                const Point<int> mouseDownPos,
                const Time& mouseDownTime,
                int numberOfClicks,
                bool mouseWasDragged) throw();

    /** Destructor. */
    ~MouseEvent() throw();

    //==============================================================================
    /** The x-position of the mouse when the event occurred.

        This value is relative to the top-left of the component to which the
        event applies (as indicated by the MouseEvent::eventComponent field).
    */
    const int x;

    /** The y-position of the mouse when the event occurred.

        This value is relative to the top-left of the component to which the
        event applies (as indicated by the MouseEvent::eventComponent field).
    */
    const int y;

    /** The key modifiers associated with the event.

        This will let you find out which mouse buttons were down, as well as which
        modifier keys were held down.

        When used for mouse-up events, this will indicate the state of the mouse buttons
        just before they were released, so that you can tell which button they let go of.
    */
    const ModifierKeys mods;

    /** The component that this event applies to.

        This is usually the component that the mouse was over at the time, but for mouse-drag
        events the mouse could actually be over a different component and the events are
        still sent to the component that the button was originally pressed on.

        The x and y member variables are relative to this component's position.

        If you use getEventRelativeTo() to retarget this object to be relative to a different
        component, this pointer will be updated, but originalComponent remains unchanged.

        @see originalComponent
    */
    Component* const eventComponent;

    /** The component that the event first occurred on.

        If you use getEventRelativeTo() to retarget this object to be relative to a different
        component, this value remains unchanged to indicate the first component that received it.

        @see eventComponent
    */
    Component* const originalComponent;

    /** The time that this mouse-event occurred.
    */
    const Time eventTime;

    /** The source device that generated this event.
    */
    MouseInputSource& source;

    //==============================================================================
    /** Returns the x co-ordinate of the last place that a mouse was pressed.

        The co-ordinate is relative to the component specified in MouseEvent::component.

        @see getDistanceFromDragStart, getDistanceFromDragStartX, mouseWasClicked
    */
    int getMouseDownX() const throw();

    /** Returns the y co-ordinate of the last place that a mouse was pressed.

        The co-ordinate is relative to the component specified in MouseEvent::component.

        @see getDistanceFromDragStart, getDistanceFromDragStartX, mouseWasClicked
    */
    int getMouseDownY() const throw();

    /** Returns the co-ordinates of the last place that a mouse was pressed.

        The co-ordinates are relative to the component specified in MouseEvent::component.

        @see getDistanceFromDragStart, getDistanceFromDragStartX, mouseWasClicked
    */
    const Point<int> getMouseDownPosition() const throw();

    /** Returns the straight-line distance between where the mouse is now and where it
        was the last time the button was pressed.

        This is quite handy for things like deciding whether the user has moved far enough
        for it to be considered a drag operation.

        @see getDistanceFromDragStartX
    */
    int getDistanceFromDragStart() const throw();

    /** Returns the difference between the mouse's current x postion and where it was
        when the button was last pressed.

        @see getDistanceFromDragStart
    */
    int getDistanceFromDragStartX() const throw();

    /** Returns the difference between the mouse's current y postion and where it was
        when the button was last pressed.

        @see getDistanceFromDragStart
    */
    int getDistanceFromDragStartY() const throw();

    /** Returns the difference between the mouse's current postion and where it was
        when the button was last pressed.

        @see getDistanceFromDragStart
    */
    const Point<int> getOffsetFromDragStart() const throw();

    /** Returns true if the mouse has just been clicked.

        Used in either your mouseUp() or mouseDrag() methods, this will tell you whether
        the user has dragged the mouse more than a few pixels from the place where the
        mouse-down occurred.

        Once they have dragged it far enough for this method to return false, it will continue
        to return false until the mouse-up, even if they move the mouse back to the same
        position where they originally pressed it. This means that it's very handy for
        objects that can either be clicked on or dragged, as you can use it in the mouseDrag()
        callback to ignore any small movements they might make while clicking.

        @returns    true if the mouse wasn't dragged by more than a few pixels between
                    the last time the button was pressed and released.
    */
    bool mouseWasClicked() const throw();

    /** For a click event, the number of times the mouse was clicked in succession.

        So for example a double-click event will return 2, a triple-click 3, etc.
    */
    int getNumberOfClicks() const throw()                               { return numberOfClicks; }

    /** Returns the time that the mouse button has been held down for.

        If called from a mouseDrag or mouseUp callback, this will return the
        number of milliseconds since the corresponding mouseDown event occurred.
        If called in other contexts, e.g. a mouseMove, then the returned value
        may be 0 or an undefined value.
    */
    int getLengthOfMousePress() const throw();

    //==============================================================================
    /** The position of the mouse when the event occurred.

        This position is relative to the top-left of the component to which the
        event applies (as indicated by the MouseEvent::eventComponent field).
    */
    const Point<int> getPosition() const throw();

    /** Returns the mouse x position of this event, in global screen co-ordinates.

        The co-ordinates are relative to the top-left of the main monitor.

        @see getScreenPosition
    */
    int getScreenX() const;

    /** Returns the mouse y position of this event, in global screen co-ordinates.

        The co-ordinates are relative to the top-left of the main monitor.

        @see getScreenPosition
    */
    int getScreenY() const;

    /** Returns the mouse position of this event, in global screen co-ordinates.

        The co-ordinates are relative to the top-left of the main monitor.

        @see getMouseDownScreenPosition
    */
    const Point<int> getScreenPosition() const;

    /** Returns the x co-ordinate at which the mouse button was last pressed.

        The co-ordinates are relative to the top-left of the main monitor.

        @see getMouseDownScreenPosition
    */
    int getMouseDownScreenX() const;

    /** Returns the y co-ordinate at which the mouse button was last pressed.

        The co-ordinates are relative to the top-left of the main monitor.

        @see getMouseDownScreenPosition
    */
    int getMouseDownScreenY() const;

    /** Returns the co-ordinates at which the mouse button was last pressed.

        The co-ordinates are relative to the top-left of the main monitor.

        @see getScreenPosition
    */
    const Point<int> getMouseDownScreenPosition() const;

    //==============================================================================
    /** Creates a version of this event that is relative to a different component.

        The x and y positions of the event that is returned will have been
        adjusted to be relative to the new component.
    */
    const MouseEvent getEventRelativeTo (Component* otherComponent) const throw();

    /** Creates a copy of this event with a different position.
        All other members of the event object are the same, but the x and y are
        replaced with these new values.
    */
    const MouseEvent withNewPosition (const Point<int>& newPosition) const throw();

    //==============================================================================
    /** Changes the application-wide setting for the double-click time limit.

        This is the maximum length of time between mouse-clicks for it to be
        considered a double-click. It's used by the Component class.

        @see getDoubleClickTimeout, MouseListener::mouseDoubleClick
    */
    static void setDoubleClickTimeout (int timeOutMilliseconds) throw();

    /** Returns the application-wide setting for the double-click time limit.

        This is the maximum length of time between mouse-clicks for it to be
        considered a double-click. It's used by the Component class.

        @see setDoubleClickTimeout, MouseListener::mouseDoubleClick
    */
    static int getDoubleClickTimeout() throw();


private:
    //==============================================================================
    const Point<int> mouseDownPos;
    const Time mouseDownTime;
    const int numberOfClicks;
    const bool wasMovedSinceMouseDown;
    static int doubleClickTimeOutMs;

    MouseEvent& operator= (const MouseEvent&);
    JUCE_LEAK_DETECTOR (MouseEvent);
};


#endif   // __JUCE_MOUSEEVENT_JUCEHEADER__
