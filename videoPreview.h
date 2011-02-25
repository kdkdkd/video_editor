#ifndef VIDEO_PREVIEW
#define VIDEO_PREVIEW

#include "juce/juce.h"
#include "localization.h"
#include "encodeVideo.h"
#include "events.h"
#include "movie.h"
#include "timeline.h"

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
    void UpdatePreview();
};
class videoPreviewComponent  : public Component, public Timer, public Thread
{
    public:
    videoPreviewComponent(encodeVideoComponent* parent);
    ~videoPreviewComponent();
    encodeVideoComponent *parent;
    void paint (Graphics& g);
    void DoOnMainComponentPointerChange();

    void  timerCallback();
    Movie *encodedMovie;
    void run();
    Timeline* timeline_copy;
    bool dirty;
    void UpdatePreview();
    String label_loading;
    int label_loading_int;
    String estimated_file_size;
};
#endif
