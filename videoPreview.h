#ifndef VIDEO_PREVIEW
#define VIDEO_PREVIEW

#include "juce/juce.h"
#include "localization.h"
#include "encodeVideo.h"
#include "events.h"
#include "movie.h"

class encodeVideoComponent;

class videoPreview  : DocumentWindow
{
    public:
    videoPreview(encodeVideoComponent* parent);
    ~videoPreview();
    void closeButtonPressed();
    void add();
    void remove();
    encodeVideoComponent *parent;
};
class videoPreviewComponent  : public Component, public Timer
{
    public:
    videoPreviewComponent(encodeVideoComponent* parent);
    ~videoPreviewComponent();
    encodeVideoComponent *parent;
    void paint (Graphics& g);
    void DoOnMainComponentPointerChange();

    void  timerCallback();
    Movie *encodedMovie;
};
#endif
