#include <ctime>
#include <iostream>
#include <opencv2/opencv.hpp> 

using namespace cv;
using namespace std;

bool grayscaleEnabled = false;

void applyGrayscaleEffect(Mat& frame) {
    if (grayscaleEnabled) {
        cvtColor(frame, frame, COLOR_BGR2GRAY);
        cvtColor(frame, frame, COLOR_GRAY2BGR);
    }
}

void updateAndDisplayFPS(Mat& frame, long& frameCounter, clock_t& lastFpsTime, string& fpsText) {
    clock_t currentTime = clock();
    double dif = double(currentTime - lastFpsTime) / CLOCKS_PER_SEC;
    if (dif >= 1.0) {
        fpsText = "FPS: " + to_string(frameCounter);
        frameCounter = 0;
        lastFpsTime = currentTime;
    }
    putText(frame, fpsText, Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);
    putText(frame, "SPACE: Grayscale | ESC: Exit", Point(10, 60), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
}

void printStatistics(clock_t totalTime, clock_t inputTime, clock_t procTime,
    clock_t outputTime, long frameCounter) {
    if (totalTime > 0 && frameCounter > 0) {
        double totalSeconds = double(totalTime) / CLOCKS_PER_SEC;
        double percentage = totalTime / 100.0;
        cout << "\n~~~~ Statistics ~~~~" << endl;
        cout << "Total time: " << totalSeconds << " seconds" << endl;
        cout << "Input time: " << (inputTime / percentage) << "%" << endl;  
        cout << "Process time: " << (procTime / percentage) << "%" << endl;      
        cout << "Output time: " << (outputTime / percentage) << "%" << endl; 
        cout << "Other time: " << ((totalTime - inputTime - procTime - outputTime) / percentage) << "%" << endl;
    }
}

void processVideoStream() {
    clock_t startTime = clock(); 
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "Error: Could not open camera" << endl;
        return;
    }
    long frameCounter = 0;      
    Mat frame;                    
    clock_t inputTime = 0;        
    clock_t procTime = 0;         
    clock_t outputTime = 0;       
    clock_t lastFpsTime = startTime; 
    string fpsText = "FPS: 0";   

    while (true) {
        clock_t inputStart = clock(); 
        cap >> frame;
        if (frame.empty()) {
            break;
        }
        inputTime += (clock() - inputStart);

        clock_t procStart = clock();
        applyGrayscaleEffect(frame);
        procTime += (clock() - procStart);
        updateAndDisplayFPS(frame, frameCounter, lastFpsTime, fpsText);
        clock_t outputStart = clock();
        imshow("Camera Feed", frame);
        frameCounter++;
        outputTime += (clock() - outputStart);

        char key = (char)waitKey(1);
        if (key == 27) {
            break;
        }
        else if (key == 32) {
            grayscaleEnabled = !grayscaleEnabled;
            cout << "Grayscale " << (grayscaleEnabled ? "ENABLED" : "DISABLED") << endl;
        }
    }
    clock_t totalTime = clock() - startTime; 
    printStatistics(totalTime, inputTime, procTime, outputTime, frameCounter);
    cap.release();
    destroyAllWindows();
}

int main(int argc, char* argv[]) {
    processVideoStream();
    return 0;
}
