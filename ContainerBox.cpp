#include "ContainerBox.h"
ContainerBox::~ContainerBox()
{
    getViewedComponent()->deleteAllChildren();
}

ContainerBox::ContainerBox(const String& name):Viewport(name)
{
    Component * movies_list_component = new Component();
    movies_list_component->setSize(1,1);
    setViewedComponent(movies_list_component);

    setScrollBarsShown(true,false);
}


void ContainerBox::resized()
{
    Viewport::resized();

    Component *child = getViewedComponent();

    int number_components = child->getNumChildComponents();
    int width_all = getMaximumVisibleWidth();
    int already_taken_width = 0;
    int already_taken_height = 0;
    int max_height = 0;

    for(int i=0; i<number_components; ++i)
    {
        Component* comp = child->getChildComponent(i);
        int comp_height = comp->getHeight();
        int comp_width = comp->getWidth();
        if(already_taken_width && width_all - already_taken_width - 8 < comp_width)
        {
            already_taken_height += max_height + 10;
            already_taken_width = 0;
            max_height = 0;
        }
        comp->setBounds(already_taken_width + 8,already_taken_height,comp_width,comp_height);
        already_taken_width += comp_width + 8;
        if(comp_height>max_height)
            max_height = comp_height;

    }

    currentHeight=already_taken_height + max_height;



     if(child->getHeight()!=currentHeight || child->getWidth()!=width_all)
            child->setSize(width_all,currentHeight);


    if(getMaximumVisibleHeight() + getViewPositionY() > currentHeight)
        setViewPosition(0,jmax(0,currentHeight - getMaximumVisibleHeight()));
}


void ContainerBox::addAndMakeVisible (Component* child, int zOrder)
{
    if (child != 0)
    {
        child->setVisible (true);
        addChildComponent (child, zOrder);
   }

}


void ContainerBox::addChildComponent (Component* child, int zOrder)
{
    getViewedComponent()->addChildComponent(child,zOrder);
    resized();
}

int ContainerBox::getIndexOfChildComponent(const Component* const child)
{

    return  getViewedComponent()->getIndexOfChildComponent(child);
}



