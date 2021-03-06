#include "include/imageFrame.hpp"
#include "include/confirmation.hpp"
#include "include/camera.hpp"
#include "include/cornerDetector.hpp"
#include <opencv/cv.h>

namespace iwb {
    ImageFrame::ImageFrame(Capture* cpt, Presentation* prs, Analysis* analysis) : Drawable("res/bg.jpg", prs, cvPoint(100, 300), cvPoint(924, 768)) {
        this->cpt = cpt;
        this->analysis = analysis;
        this->captureCb = NULL;

        ulImage = cvLoadImage("res/CleftSmaller.jpg", 0);
        brImage = cvLoadImage("res/CrightSmaller.jpg", 0);

        startTime = -1;
        frameCounter = 0;
        currentProcess = DRAWING;
        captureState = GETTING_CAPTURE;

        // initializing camera coordinates
        // FIXME: replace with actual camera coordinates
        cameraUL = cvPoint(0, 0);
        cameraBR = cvPoint(640, 480);
//        refreshCornerCoords(time);

//        IplImage* currentFrame = cpt->getCapture();
//                    p1 = Analysis::getLocation(Analysis::getDiff(cornerFrame, currentFrame), tmp1, true);
//                    p2 = Analysis::getLocation(Analysis::getDiff(cornerFrame, currentFrame), tmp2, false);
    }

    void ImageFrame::setCaptureCallback(boost::function<void (void)> captureCb) {
        this->captureCb = captureCb;
    }

    void ImageFrame::refreshCornerCoords(IplImage* currentFrame) {
//        bool calculate = true;
//        while (firstTime || calculate) {
//            CvPoint ul = Analysis::getLocation(analysis->getCornerDiff(), ulImage, true);
//            CvPoint br = Analysis::getLocation(analysis->getCornerDiff(), brImage, false);
//
//            if (ul.x < br.x && ul.y < br.y) {
//                projectorUL = ul;
//                projectorBR = br;
//
//                break;
//            }
//
//            calculate = false;
//        }
                        // Second time capturing still frame

        CornerDetector cd;
        IplImage* diff = analysis->getCornerDiff(capturedFrame, currentFrame);
        if (!cd.detect(diff, &cameraUL, &cameraBR)) {
            printf("DEBUG: IT'S FAAAAAAAAAAAALSE YOU KNOW\n");
//            cvSaveImage("cornercornerCAPTURED.jpg", capturedFrame);
//            cvSaveImage("cornercornerCURRENT.jpg", currentFrame);
//            cvSaveImage("cornercornercorner.jpg", diff);

                        printf("if ul.x: %d, ul.y: %d\n", projectorUL.x, projectorUL.y);
                        printf("if br.x: %d, br.y: %d\n", projectorBR.x, projectorBR.y);
            return;
        }
//                        cameraUL = analysis->getLocation(diff, ulImage, true);
//                        cameraBR = analysis->getLocation(diff, brImage, false);

        Camera *camera = Camera::getInstance();
        CvPoint po = camera->getProjectorOrigin();
        int pw = camera->getProjectorWidth(),
            ph = camera->getProjectorHeight(),
            w  = this->prs->screenWidth,
            h  = this->prs->screenHeight,
        x = cameraUL.x,
        y = cameraUL.y;

        //printf("\n\nx: po.x(%d) w(%d) pw(%d) p.x(%d)\ny: po.y(%d) h(%d) ph(%d) p.y(%d)\n", po.x, w, pw, p.x, po.y, h, ph, p.y);
//        int x = po.x+((float)pw/w)*p.x,
//            y = po.y+((float)ph/h)*p.y;
        projectorUL.x = (int)((float)x - po.x)* w / pw;
        projectorUL.y = (int)((float)y - po.y)* h / ph;
        x = cameraBR.x,
        y = cameraBR.y;
        projectorBR.x = (int)((float)x - po.x)* w / pw;
        projectorBR.y = (int)((float)y - po.y)* h / ph;
        //printf("(%d, %d)", x,y);
//        return cvPoint(x,y);
//                        cvRectangle(diff, projectorUL, projectorBR, cvScalar(0, 0, 255, 0), 1, 0, 0);
//        cpt->saveFrame("cornerDiff.jpg", diff);

                        printf("if ul.x: %d, ul.y: %d\n", projectorUL.x, projectorUL.y);
                        printf("if br.x: %d, br.y: %d\n", projectorBR.x, projectorBR.y);
//                        cpt->saveFrame("diff.jpg", Analysis::getDiff(capturedFrame, currentFrame));
                        // ]]] Save KF & CI
                        //------------------------------------------------------------------------------
    }

    void ImageFrame::captureFrame(IplImage* currentFrame) {
//                        cvRectangle(currentFrame, cameraUL, cameraBR, cvScalar(0, 0, 255, 0), 1, 0, 0);

                        //------------------------------------------------------------------------------
                        // [[[ Save KF & CI
                        //Counting coordinates for making CI
                        int width = cameraBR.x - cameraUL.x,
                                height = cameraBR.y - cameraUL.y;
                        try {
                            cvSetImageROI(currentFrame, cvRect(cameraUL.x, cameraUL.y, width, height));
                            IplImage* Ci = cvCreateImage(cvGetSize(currentFrame), currentFrame->depth, currentFrame->nChannels);

                            cvCopyImage(currentFrame, Ci);
                            capturedFrame = cvCloneImage(Ci);
//                            if (currentFrame != NULL)
//                                cvReleaseImage(&currentFrame);

                            cvResetImageROI(currentFrame);
//                            // Saving Images
//                            // @todo save function here
//                            cpt->saveFrame("KeyFrame.jpg", currentFrame);
//                            //currentKfSaved = true;
//
//                            isSaved = true;
                            //cvReleaseImage(&capturedFrame);
                            cvReleaseImage(&Ci);
                        } catch (cv::Exception& e) {

                        }

    }

    void ImageFrame::setImagePath(const char* imagePath) {
        if ((captureState == GETTING_CAPTURE || captureState == WAITING_FOR_CORNERS) && currentProcess == DRAWING) {
            cvReleaseImage(&image);
            image = cvLoadImage(imagePath, CV_LOAD_IMAGE_UNCHANGED);
            currentProcess = CHANGING_IMAGE;
//            captureState = GETTING_CAPTURE;
        }
    }

    void ImageFrame::saveFrame() {
        if ((captureState == GETTING_CAPTURE || captureState == WAITING_FOR_CORNERS) && currentProcess == DRAWING) {
            currentProcess = SAVING_IMAGE;
//            captureState = GETTING_CAPTURE;
        }
    }

    void ImageFrame::checkForCapture() {
        refreshCornerCoords(NULL);
    }

    void ImageFrame::checkForMovement() {
//        if (captureState == IDLE) {
//            return;
//        }

        IplImage* currentFrame = cvQueryFrame(cpt->getCapture());
        if (cpt->getPreviousFrame() == NULL) {
            cpt->setPreviousFrame(cvCloneImage(currentFrame));
            return;
        }
        
        IplImage* previousFrame = cpt->getPreviousFrame();

        IplImage* diff = analysis->getCornerDiff(previousFrame, currentFrame);
        
        cpt->setPreviousFrame(cvCloneImage(currentFrame));

//        int startTime = -1;

        if (!analysis->isMoving(diff) && (captureState == GETTING_CAPTURE || captureState == WAITING_FOR_CORNERS)) {
            printf("DEBUG: not moving\n");
//            if (startTime == -1) {
//                printf("DEBUG: clock started\n");
//                startTime = clock() / CLOCKS_PER_SEC;
//            }
            frameCounter++;

            if (frameCounter >= 2) {
                    capturedFrame = cvCloneImage(currentFrame);
                switch(captureState) {
                    case GETTING_CAPTURE:
                        puts("first");
                        frameCounter = 0;
                        captureState = WAITING_FOR_CORNERS;
                        break;
                    case WAITING_FOR_CORNERS:
                        refreshCornerCoords(currentFrame);
                        puts("second");
                        frameCounter = 0;
                        captureState = CAPTURED;
                }
            }
//            int timeDifference;
//            timeDifference = clock() / CLOCKS_PER_SEC - startTime;
//            if (timeDifference >= 2) {
//                printf("DEBUG: clock triggered\n");
//                switch (captureState) {
//                    case GETTING_CAPTURE:
//                        capturedFrame = cvCloneImage(currentFrame);
//                        puts("first");
//                        startTime = -1;
//                        captureState = WAITING_FOR_CORNERS;
//                        break;
//                    case WAITING_FOR_CORNERS:
//                        refreshCornerCoords(currentFrame);
//                        puts("second");
//                        // TODO: pass the saveFrame function as a callback to the confirmation dialog for touched yes event
//                        captureState = CAPTURED;
//                        break;
//                }
//            }
        } else {
            frameCounter = 0;
        }

//        if (!analysis->isMoving(diff) && (captureState == GETTING_CAPTURE || captureState == WAITING_FOR_CORNERS)) {
//            printf("DEBUG: not moving\n");
//            if (startTime == -1) {
//                printf("DEBUG: clock started\n");
//                startTime = clock() / CLOCKS_PER_SEC;
//            }
//            int timeDifference;
//            timeDifference = clock() / CLOCKS_PER_SEC - startTime;
//            if (timeDifference >= 2) {
//                printf("DEBUG: clock triggered\n");
//                switch (captureState) {
//                    case GETTING_CAPTURE:
//                        capturedFrame = cvCloneImage(currentFrame);
//                        puts("first");
//                        startTime = -1;
//                        captureState = WAITING_FOR_CORNERS;
//                        break;
//                    case WAITING_FOR_CORNERS:
//                        refreshCornerCoords(currentFrame);
//                        puts("second");
//                        // TODO: pass the saveFrame function as a callback to the confirmation dialog for touched yes event
//                        captureState = CAPTURED;
//                        break;
//                }
//            }
//        } else {
//            startTime = -1;
//        }

        cvReleaseImage(&diff);
    }

    void ImageFrame::update() {
//        checkForMovement();
        printf("DEBUG: imageFrame current process: %d\n", currentProcess);
        printf("DEBUG: imageFrame capture state: %d\n", captureState);

        checkForMovement();
        switch (currentProcess) {
            case DRAWING:
                break;
            case CHANGING_IMAGE:
                checkForMovement();
                if (captureState == CAPTURED) {

                    currentProcess = DRAWING;
                }
                break;
            case SAVING_IMAGE:
                checkForMovement();
                if (captureState == CAPTURED) {
                    captureFrame(cpt->getPreviousFrame());
                    
                    char filepath[80];
                    sprintf(filepath, "tmp/1/capture_%d.jpg", (int)clock());
                    cpt->saveFrame(filepath, capturedFrame);
                            if (captureCb != NULL) {
                                captureCb();
                            }

                    currentProcess = DRAWING;
                }
                break;
        }

        if (captureState == CAPTURED) {
            captureState = WAITING_FOR_CORNERS;
            startTime = -1;
            cvReleaseImage(&capturedFrame);
            capturedFrame = NULL;
        }
    }

    void ImageFrame::draw(Presentation* prs) {
        if (currentProcess == DRAWING || currentProcess == SAVING_IMAGE) {
//            prs->putImage(projectorUL, projectorBR, image);
            prs->putImage(projectorUL, projectorBR, NULL, NULL, image);
//            prs->applyBuffer();
        }
    }
}
