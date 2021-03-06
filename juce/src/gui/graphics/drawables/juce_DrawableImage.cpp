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

#include "../../../core/juce_StandardHeader.h"

BEGIN_JUCE_NAMESPACE

#include "juce_DrawableImage.h"
#include "juce_DrawableComposite.h"
#include "../imaging/juce_ImageCache.h"


//==============================================================================
DrawableImage::DrawableImage()
    : image (0),
      opacity (1.0f),
      overlayColour (0x00000000)
{
    bounds.topRight = RelativePoint (Point<float> (1.0f, 0.0f));
    bounds.bottomLeft = RelativePoint (Point<float> (0.0f, 1.0f));
}

DrawableImage::DrawableImage (const DrawableImage& other)
    : image (other.image),
      opacity (other.opacity),
      overlayColour (other.overlayColour),
      bounds (other.bounds)
{
}

DrawableImage::~DrawableImage()
{
}

//==============================================================================
void DrawableImage::setImage (const Image& imageToUse)
{
    image = imageToUse;

    setBounds (imageToUse.getBounds());

    if (image.isValid())
    {
        bounds.topLeft = RelativePoint (Point<float> (0.0f, 0.0f));
        bounds.topRight = RelativePoint (Point<float> ((float) image.getWidth(), 0.0f));
        bounds.bottomLeft = RelativePoint (Point<float> (0.0f, (float) image.getHeight()));
    }

    refreshTransformFromBounds();
}

void DrawableImage::setOpacity (const float newOpacity)
{
    opacity = newOpacity;
}

void DrawableImage::setOverlayColour (const Colour& newOverlayColour)
{
    overlayColour = newOverlayColour;
}

void DrawableImage::setBoundingBox (const RelativeParallelogram& newBounds)
{
    bounds = newBounds;
    refreshTransformFromBounds();
}

//==============================================================================
void DrawableImage::refreshTransformFromBounds()
{
    if (! image.isNull())
    {
        Point<float> resolved[3];
        bounds.resolveThreePoints (resolved, getParent());

        const Point<float> tr (resolved[0] + (resolved[1] - resolved[0]) / (float) image.getWidth());
        const Point<float> bl (resolved[0] + (resolved[2] - resolved[0]) / (float) image.getHeight());

        AffineTransform t (AffineTransform::fromTargetPoints (resolved[0].getX(), resolved[0].getY(),
                                                              tr.getX(), tr.getY(),
                                                              bl.getX(), bl.getY()));

        if (! t.isSingularity())
            setTransform (t);
    }
}

//==============================================================================
void DrawableImage::paint (Graphics& g)
{
    if (image.isValid())
    {
        if (opacity > 0.0f && ! overlayColour.isOpaque())
        {
            g.setOpacity (opacity);
            g.drawImageAt (image, 0, 0, false);
        }

        if (! overlayColour.isTransparent())
        {
            g.setColour (overlayColour.withMultipliedAlpha (opacity));
            g.drawImageAt (image, 0, 0, true);
        }
    }
}

const Rectangle<float> DrawableImage::getDrawableBounds() const
{
    return image.getBounds().toFloat();
}

bool DrawableImage::hitTest (int x, int y) const
{
    return (! image.isNull())
            && image.getPixelAt (x, y).getAlpha() >= 127;
}

Drawable* DrawableImage::createCopy() const
{
    return new DrawableImage (*this);
}

//==============================================================================
const Identifier DrawableImage::valueTreeType ("Image");

const Identifier DrawableImage::ValueTreeWrapper::opacity ("opacity");
const Identifier DrawableImage::ValueTreeWrapper::overlay ("overlay");
const Identifier DrawableImage::ValueTreeWrapper::image ("image");
const Identifier DrawableImage::ValueTreeWrapper::topLeft ("topLeft");
const Identifier DrawableImage::ValueTreeWrapper::topRight ("topRight");
const Identifier DrawableImage::ValueTreeWrapper::bottomLeft ("bottomLeft");

//==============================================================================
DrawableImage::ValueTreeWrapper::ValueTreeWrapper (const ValueTree& state_)
    : ValueTreeWrapperBase (state_)
{
    jassert (state.hasType (valueTreeType));
}

const var DrawableImage::ValueTreeWrapper::getImageIdentifier() const
{
    return state [image];
}

Value DrawableImage::ValueTreeWrapper::getImageIdentifierValue (UndoManager* undoManager)
{
    return state.getPropertyAsValue (image, undoManager);
}

void DrawableImage::ValueTreeWrapper::setImageIdentifier (const var& newIdentifier, UndoManager* undoManager)
{
    state.setProperty (image, newIdentifier, undoManager);
}

float DrawableImage::ValueTreeWrapper::getOpacity() const
{
    return (float) state.getProperty (opacity, 1.0);
}

Value DrawableImage::ValueTreeWrapper::getOpacityValue (UndoManager* undoManager)
{
    if (! state.hasProperty (opacity))
        state.setProperty (opacity, 1.0, undoManager);

    return state.getPropertyAsValue (opacity, undoManager);
}

void DrawableImage::ValueTreeWrapper::setOpacity (float newOpacity, UndoManager* undoManager)
{
    state.setProperty (opacity, newOpacity, undoManager);
}

const Colour DrawableImage::ValueTreeWrapper::getOverlayColour() const
{
    return Colour (state [overlay].toString().getHexValue32());
}

void DrawableImage::ValueTreeWrapper::setOverlayColour (const Colour& newColour, UndoManager* undoManager)
{
    if (newColour.isTransparent())
        state.removeProperty (overlay, undoManager);
    else
        state.setProperty (overlay, String::toHexString ((int) newColour.getARGB()), undoManager);
}

Value DrawableImage::ValueTreeWrapper::getOverlayColourValue (UndoManager* undoManager)
{
    return state.getPropertyAsValue (overlay, undoManager);
}

const RelativeParallelogram DrawableImage::ValueTreeWrapper::getBoundingBox() const
{
    return RelativeParallelogram (state.getProperty (topLeft, "0, 0"),
                                  state.getProperty (topRight, "100, 0"),
                                  state.getProperty (bottomLeft, "0, 100"));
}

void DrawableImage::ValueTreeWrapper::setBoundingBox (const RelativeParallelogram& newBounds, UndoManager* undoManager)
{
    state.setProperty (topLeft, newBounds.topLeft.toString(), undoManager);
    state.setProperty (topRight, newBounds.topRight.toString(), undoManager);
    state.setProperty (bottomLeft, newBounds.bottomLeft.toString(), undoManager);
}


//==============================================================================
void DrawableImage::refreshFromValueTree (const ValueTree& tree, ImageProvider* imageProvider)
{
    const ValueTreeWrapper controller (tree);
    setName (controller.getID());

    const float newOpacity = controller.getOpacity();
    const Colour newOverlayColour (controller.getOverlayColour());

    Image newImage;
    const var imageIdentifier (controller.getImageIdentifier());

    jassert (imageProvider != 0 || imageIdentifier.isVoid()); // if you're using images, you need to provide something that can load and save them!

    if (imageProvider != 0)
        newImage = imageProvider->getImageForIdentifier (imageIdentifier);

    const RelativeParallelogram newBounds (controller.getBoundingBox());

    if (newOpacity != opacity || overlayColour != newOverlayColour || image != newImage)
    {
        repaint();
        opacity = newOpacity;
        overlayColour = newOverlayColour;
        bounds = newBounds;
        setImage (newImage);
    }
}

const ValueTree DrawableImage::createValueTree (ImageProvider* imageProvider) const
{
    ValueTree tree (valueTreeType);
    ValueTreeWrapper v (tree);

    v.setID (getName(), 0);
    v.setOpacity (opacity, 0);
    v.setOverlayColour (overlayColour, 0);
    v.setBoundingBox (bounds, 0);

    if (image.isValid())
    {
        jassert (imageProvider != 0); // if you're using images, you need to provide something that can load and save them!

        if (imageProvider != 0)
            v.setImageIdentifier (imageProvider->getIdentifierForImage (image), 0);
    }

    return tree;
}


END_JUCE_NAMESPACE
