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

#ifndef __JUCE_TOOLBARITEMCOMPONENT_JUCEHEADER__
#define __JUCE_TOOLBARITEMCOMPONENT_JUCEHEADER__

#include "../buttons/juce_Button.h"
#include "../../graphics/drawables/juce_Drawable.h"
#include "juce_Toolbar.h"
class ItemDragAndDropOverlayComponent;


//==============================================================================
/**
    A component that can be used as one of the items in a Toolbar.

    Each of the items on a toolbar must be a component derived from ToolbarItemComponent,
    and these objects are always created by a ToolbarItemFactory - see the ToolbarItemFactory
    class for further info about creating them.

    The ToolbarItemComponent class is actually a button, but can be used to hold non-button
    components too. To do this, set the value of isBeingUsedAsAButton to false when
    calling the constructor, and override contentAreaChanged(), in which you can position
    any sub-components you need to add.

    To add basic buttons without writing a special subclass, have a look at the
    ToolbarButton class.

    @see ToolbarButton, Toolbar, ToolbarItemFactory
*/
class JUCE_API  ToolbarItemComponent  : public Button
{
public:
    //==============================================================================
    /** Constructor.

        @param itemId       the ID of the type of toolbar item which this represents
        @param labelText    the text to display if the toolbar's style is set to
                            Toolbar::iconsWithText or Toolbar::textOnly
        @param isBeingUsedAsAButton     set this to false if you don't want the button
                            to draw itself with button over/down states when the mouse
                            moves over it or clicks
    */
    ToolbarItemComponent (int itemId,
                          const String& labelText,
                          bool isBeingUsedAsAButton);

    /** Destructor. */
    ~ToolbarItemComponent();

    //==============================================================================
    /** Returns the item type ID that this component represents.
        This value is in the constructor.
    */
    int getItemId() const throw()                                       { return itemId; }

    /** Returns the toolbar that contains this component, or 0 if it's not currently
        inside one.
    */
    Toolbar* getToolbar() const;

    /** Returns true if this component is currently inside a toolbar which is vertical.
        @see Toolbar::isVertical
    */
    bool isToolbarVertical() const;

    /** Returns the current style setting of this item.

        Styles are listed in the Toolbar::ToolbarItemStyle enum.
        @see setStyle, Toolbar::getStyle
    */
    Toolbar::ToolbarItemStyle getStyle() const throw()                  { return toolbarStyle; }

    /** Changes the current style setting of this item.

        Styles are listed in the Toolbar::ToolbarItemStyle enum, and are automatically updated
        by the toolbar that holds this item.

        @see setStyle, Toolbar::setStyle
    */
    virtual void setStyle (const Toolbar::ToolbarItemStyle& newStyle);

    /** Returns the area of the component that should be used to display the button image or
        other contents of the item.

        This content area may change when the item's style changes, and may leave a space around the
        edge of the component where the text label can be shown.

        @see contentAreaChanged
    */
    const Rectangle<int> getContentArea() const throw()                 { return contentArea; }

    //==============================================================================
    /** This method must return the size criteria for this item, based on a given toolbar
        size and orientation.

        The preferredSize, minSize and maxSize values must all be set by your implementation
        method. If the toolbar is horizontal, these will be the width of the item; for a vertical
        toolbar, they refer to the item's height.

        The preferredSize is the size that the component would like to be, and this must be
        between the min and max sizes. For a fixed-size item, simply set all three variables to
        the same value.

        The toolbarThickness parameter tells you the depth of the toolbar - the same as calling
        Toolbar::getThickness().

        The isToolbarVertical parameter tells you whether the bar is oriented horizontally or
        vertically.
    */
    virtual bool getToolbarItemSizes (int toolbarThickness,
                                      bool isToolbarVertical,
                                      int& preferredSize,
                                      int& minSize,
                                      int& maxSize) = 0;

    /** Your subclass should use this method to draw its content area.

        The graphics object that is passed-in will have been clipped and had its origin
        moved to fit the content area as specified get getContentArea(). The width and height
        parameters are the width and height of the content area.

        If the component you're writing isn't a button, you can just do nothing in this method.
    */
    virtual void paintButtonArea (Graphics& g,
                                  int width, int height,
                                  bool isMouseOver, bool isMouseDown) = 0;

    /** Callback to indicate that the content area of this item has changed.

        This might be because the component was resized, or because the style changed and
        the space needed for the text label is different.

        See getContentArea() for a description of what the area is.
    */
    virtual void contentAreaChanged (const Rectangle<int>& newBounds) = 0;


    //==============================================================================
    /** Editing modes.
        These are used by setEditingMode(), but will be rarely needed in user code.
    */
    enum ToolbarEditingMode
    {
        normalMode = 0,     /**< Means that the component is active, inside a toolbar. */
        editableOnToolbar,  /**< Means that the component is on a toolbar, but the toolbar is in
                                 customisation mode, and the items can be dragged around. */
        editableOnPalette   /**< Means that the component is on an new-item palette, so it can be
                                 dragged onto a toolbar to add it to that bar.*/
    };

    /** Changes the editing mode of this component.

        This is used by the ToolbarItemPalette and related classes for making the items draggable,
        and is unlikely to be of much use in end-user-code.
    */
    void setEditingMode (const ToolbarEditingMode newMode);

    /** Returns the current editing mode of this component.

        This is used by the ToolbarItemPalette and related classes for making the items draggable,
        and is unlikely to be of much use in end-user-code.
    */
    ToolbarEditingMode getEditingMode() const throw()                   { return mode; }


    //==============================================================================
    /** @internal */
    void paintButton (Graphics& g, bool isMouseOver, bool isMouseDown);
    /** @internal */
    void resized();

private:
    friend class Toolbar;
    friend class ItemDragAndDropOverlayComponent;
    const int itemId;
    ToolbarEditingMode mode;
    Toolbar::ToolbarItemStyle toolbarStyle;
    ScopedPointer <Component> overlayComp;
    int dragOffsetX, dragOffsetY;
    bool isActive, isBeingDragged, isBeingUsedAsAButton;
    Rectangle<int> contentArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToolbarItemComponent);
};


#endif   // __JUCE_TOOLBARITEMCOMPONENT_JUCEHEADER__
