#include <opencv/highgui.h>
#include "utils.h"
#include "OutputMethods.h"
#include "MainGazeTracker.h"

#define MAIN_WINDOW_NAME "OpenGazer"

MainGazeTracker* gazeTracker;

static vector<shared_ptr<AbstractStore> > getStores() {
  vector<shared_ptr<AbstractStore> > stores;

  stores.push_back( shared_ptr<AbstractStore>( new SocketStore() ) );
  stores.push_back( shared_ptr<AbstractStore>( new StreamStore(cout) ) );
  // stores.push_back(shared_ptr<AbstractStore>
  //   (new WindowStore(WindowPointer::PointerSpec(20, 20, 0, 0, 1),
  //                   WindowPointer::PointerSpec(30, 30, 1, 0, 1))));

  return stores;
}

// Redundant work to wrap the buttons
// TODO: Abstract this using macros
void calibrateCallbackWrapper(int state, void*) {
  if(state == -1) {       // for push buttons
    gazeTracker->startCalibration();
  }
}

void testCallbackWrapper(int state, void*) {
  if(state == -1) {       // for push buttons
    gazeTracker->startTesting();
  }
}

void savePointsCallbackWrapper(int state, void*) {
  if(state == -1) {       // for push buttons
    gazeTracker->savepoints();
  }
}

void loadPointsCallbackWrapper(int state, void*) {
  if(state == -1) {       // for push buttons
    gazeTracker->loadpoints();
  }
}

void clearPointsCallbackWrapper(int state, void*) {
  if(state == -1) {       // for push buttons
    gazeTracker->clearpoints();
  }
}

void mouseClick(int event, int x, int y, int flags, void* param) {
  if(event == CV_EVENT_LBUTTONDOWN || event == CV_EVENT_LBUTTONDBLCLK) {
    Point point(x, y);
    PointTracker &tracker = gazeTracker->tracking->tracker;
    int closest = tracker.getClosestTracker(point);
    int lastPointId;

    if(closest >= 0 && point.distance(tracker.currentpoints[closest]) <= 10) lastPointId = closest;
    else
      lastPointId = -1;

    if(event == CV_EVENT_LBUTTONDOWN) {
      if(lastPointId >= 0)
        tracker.updatetracker(lastPointId, point);
      else {
        tracker.addtracker(point);
      }

    }
    if(event == CV_EVENT_LBUTTONDBLCLK) { 
      if(lastPointId >= 0)
        tracker.removetracker(lastPointId);      
    }
  }
}

void createButtons() {
  //Create the buttons
  cvCreateButton("Calibrate", calibrateCallbackWrapper);
  cvCreateButton("Test", testCallbackWrapper);
  cvCreateButton("Save Points", savePointsCallbackWrapper);
  cvCreateButton("Load Points", loadPointsCallbackWrapper);
  cvCreateButton("Clear Points", clearPointsCallbackWrapper);
}

void registerMouseCallbacks() {
  cvSetMouseCallback(MAIN_WINDOW_NAME, mouseClick, NULL);
}

void drawFrame() {
  cvShowImage( MAIN_WINDOW_NAME, gazeTracker->canvas.get() );
}

int main(int argc, char **argv) {
  gazeTracker = new MainGazeTracker( argc, argv, getStores() );

  cvNamedWindow(MAIN_WINDOW_NAME, CV_GUI_EXPANDED);
  cvResizeWindow(MAIN_WINDOW_NAME, 640, 480);

  createButtons();
  registerMouseCallbacks();

  while(1) {
    gazeTracker->doprocessing();

    drawFrame();

    char c = cvWaitKey(33);
    if(c == 27) break;
  }

  cvDestroyWindow(MAIN_WINDOW_NAME);
  delete gazeTracker;
  return 0;
}
