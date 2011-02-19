#ifndef VIDEO_PREVIEW
#define VIDEO_PREVIEW

#include "juce/juce.h"
#include "localization.h"
#include "encodeVideo.h"

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
#endif
