#include <vector>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
std::vector<PossiblePlate> detectPlatesInScene(cv::Mat &imgOriginalScene) {
    std::vector<PossiblePlate> vectorOfPossiblePlates;			// this will be the return value

    cv::Mat imgGrayscaleScene;
    cv::Mat imgThreshScene;
    cv::Mat imgContours(imgOriginalScene.size(), CV_8UC3, SCALAR_BLACK);

    cv::RNG rng;

    cv::destroyAllWindows();


    cv::imshow("0", imgOriginalScene);


    preprocess(imgOriginalScene, imgGrayscaleScene, imgThreshScene);        // preprocess to get grayscale and threshold images


    cv::imshow("1a", imgGrayscaleScene);
    cv::imshow("1b", imgThreshScene);


    // find all possible chars in the scene,
    // this function first finds all contours, then only includes contours that could be chars (without comparison to other chars yet)
    std::vector<PossibleChar> vectorOfPossibleCharsInScene = findPossibleCharsInScene(imgThreshScene);


    std::cout << "step 2 - vectorOfPossibleCharsInScene.Count = " << vectorOfPossibleCharsInScene.size() << std::endl;        // 131 with MCLRNF1 image

    imgContours = cv::Mat(imgOriginalScene.size(), CV_8UC3, SCALAR_BLACK);
    std::vector<std::vector<cv::Point> > contours;

    for (auto &possibleChar : vectorOfPossibleCharsInScene) {
        contours.push_back(possibleChar.contour);
    }
    cv::drawContours(imgContours, contours, -1, SCALAR_WHITE);
    cv::imshow("2b", imgContours);


    // given a vector of all possible chars, find groups of matching chars
    // in the next steps each group of matching chars will attempt to be recognized as a plate
    std::vector<std::vector<PossibleChar> > vectorOfVectorsOfMatchingCharsInScene = findVectorOfVectorsOfMatchingChars(vectorOfPossibleCharsInScene);


    std::cout << "step 3 - vectorOfVectorsOfMatchingCharsInScene.size() = " << vectorOfVectorsOfMatchingCharsInScene.size() << std::endl;        // 13 with MCLRNF1 image

    imgContours = cv::Mat(imgOriginalScene.size(), CV_8UC3, SCALAR_BLACK);

    for (auto &vectorOfMatchingChars : vectorOfVectorsOfMatchingCharsInScene) {
        int intRandomBlue = rng.uniform(0, 256);
        int intRandomGreen = rng.uniform(0, 256);
        int intRandomRed = rng.uniform(0, 256);

        std::vector<std::vector<cv::Point> > contours;

        for (auto &matchingChar : vectorOfMatchingChars) {
            contours.push_back(matchingChar.contour);
        }
        cv::drawContours(imgContours, contours, -1, cv::Scalar((double)intRandomBlue, (double)intRandomGreen, (double)intRandomRed));
    }
    cv::imshow("3", imgContours);


    for (auto &vectorOfMatchingChars : vectorOfVectorsOfMatchingCharsInScene) {                     // for each group of matching chars
        PossiblePlate possiblePlate = extractPlate(imgOriginalScene, vectorOfMatchingChars);        // attempt to extract plate

        if (possiblePlate.imgPlate.empty() == false) {                                              // if plate was found
            vectorOfPossiblePlates.push_back(possiblePlate);                                        // add to vector of possible plates
        }
    }

    std::cout << std::endl << vectorOfPossiblePlates.size() << " possible plates found" << std::endl;       // 13 with MCLRNF1 image


    std::cout << std::endl;
    cv::imshow("4a", imgContours);

    for (unsigned int i = 0; i < vectorOfPossiblePlates.size(); i++) {
        cv::Point2f p2fRectPoints[4];

        vectorOfPossiblePlates[i].rrLocationOfPlateInScene.points(p2fRectPoints);

        for (int j = 0; j < 4; j++) {
            cv::line(imgContours, p2fRectPoints[j], p2fRectPoints[(j + 1) % 4], SCALAR_RED, 2);
        }
        cv::imshow("4a", imgContours);

        std::cout << "possible plate " << i << ", click on any image and press a key to continue . . ." << std::endl;

        cv::imshow("4b", vectorOfPossiblePlates[i].imgPlate);
        cv::waitKey(0);
    }
    std::cout << std::endl << "plate detection complete, click on any image and press a key to begin char recognition . . ." << std::endl << std::endl;
    cv::waitKey(0);


    return vectorOfPossiblePlates;
}
PossiblePlate extractPlate(cv::Mat &imgOriginal, std::vector<PossibleChar> &vectorOfMatchingChars) {
    PossiblePlate possiblePlate;            // this will be the return value

   // sort chars from left to right based on x position
    std::sort(vectorOfMatchingChars.begin(), vectorOfMatchingChars.end(), PossibleChar::sortCharsLeftToRight);

    // calculate the center point of the plate
    double dblPlateCenterX = (double)(vectorOfMatchingChars[0].intCenterX + vectorOfMatchingChars[vectorOfMatchingChars.size() - 1].intCenterX) / 2.0;
    double dblPlateCenterY = (double)(vectorOfMatchingChars[0].intCenterY + vectorOfMatchingChars[vectorOfMatchingChars.size() - 1].intCenterY) / 2.0;
    cv::Point2d p2dPlateCenter(dblPlateCenterX, dblPlateCenterY);

    // calculate plate width and height
    int intPlateWidth = (int)((vectorOfMatchingChars[vectorOfMatchingChars.size() - 1].boundingRect.x + vectorOfMatchingChars[vectorOfMatchingChars.size() - 1].boundingRect.width - vectorOfMatchingChars[0].boundingRect.x) * PLATE_WIDTH_PADDING_FACTOR);

    double intTotalOfCharHeights = 0;

    for (auto &matchingChar : vectorOfMatchingChars) {
        intTotalOfCharHeights = intTotalOfCharHeights + matchingChar.boundingRect.height;
    }

    double dblAverageCharHeight = (double)intTotalOfCharHeights / vectorOfMatchingChars.size();

    int intPlateHeight = (int)(dblAverageCharHeight * PLATE_HEIGHT_PADDING_FACTOR);

    // calculate correction angle of plate region
    double dblOpposite = vectorOfMatchingChars[vectorOfMatchingChars.size() - 1].intCenterY - vectorOfMatchingChars[0].intCenterY;
    double dblHypotenuse = distanceBetweenChars(vectorOfMatchingChars[0], vectorOfMatchingChars[vectorOfMatchingChars.size() - 1]);
    double dblCorrectionAngleInRad = asin(dblOpposite / dblHypotenuse);
    double dblCorrectionAngleInDeg = dblCorrectionAngleInRad * (180.0 / CV_PI);

    // assign rotated rect member variable of possible plate
    possiblePlate.rrLocationOfPlateInScene = cv::RotatedRect(p2dPlateCenter, cv::Size2f((float)intPlateWidth, (float)intPlateHeight), (float)dblCorrectionAngleInDeg);

    cv::Mat rotationMatrix;             // final steps are to perform the actual rotation
    cv::Mat imgRotated;
    cv::Mat imgCropped;

    rotationMatrix = cv::getRotationMatrix2D(p2dPlateCenter, dblCorrectionAngleInDeg, 1.0);         // get the rotation matrix for our calculated correction angle

    cv::warpAffine(imgOriginal, imgRotated, rotationMatrix, imgOriginal.size());            // rotate the entire image

                                                                                            // crop out the actual plate portion of the rotated image
    cv::getRectSubPix(imgRotated, possiblePlate.rrLocationOfPlateInScene.size, possiblePlate.rrLocationOfPlateInScene.center, imgCropped);

    possiblePlate.imgPlate = imgCropped;            // copy the cropped plate image into the applicable member variable of the possible plate

    return(possiblePlate);
}

std::vector<PossibleChar> findVectorOfMatchingChars(const PossibleChar &possibleChar, const std::vector<PossibleChar> &vectorOfChars) {
    // the purpose of this function is, given a possible char and a big vector of possible chars,
    // find all chars in the big vector that are a match for the single possible char, and return those matching chars as a vector
    std::vector<PossibleChar> vectorOfMatchingChars;                // this will be the return value

    for (auto &possibleMatchingChar : vectorOfChars) {              // for each char in big vector

                                                                    // if the char we attempting to find matches for is the exact same char as the char in the big vector we are currently checking
        if (possibleMatchingChar == possibleChar) {
            // then we should not include it in the vector of matches b/c that would end up double including the current char
            continue;           // so do not add to vector of matches and jump back to top of for loop
        }
        // compute stuff to see if chars are a match
        double dblDistanceBetweenChars = distanceBetweenChars(possibleChar, possibleMatchingChar);
        double dblAngleBetweenChars = angleBetweenChars(possibleChar, possibleMatchingChar);
        double dblChangeInArea = (double)abs(possibleMatchingChar.boundingRect.area() - possibleChar.boundingRect.area()) / (double)possibleChar.boundingRect.area();
        double dblChangeInWidth = (double)abs(possibleMatchingChar.boundingRect.width - possibleChar.boundingRect.width) / (double)possibleChar.boundingRect.width;
        double dblChangeInHeight = (double)abs(possibleMatchingChar.boundingRect.height - possibleChar.boundingRect.height) / (double)possibleChar.boundingRect.height;

        // check if chars match
        if (dblDistanceBetweenChars < (possibleChar.dblDiagonalSize * MAX_DIAG_SIZE_MULTIPLE_AWAY) &&
            dblAngleBetweenChars < MAX_ANGLE_BETWEEN_CHARS &&
            dblChangeInArea < MAX_CHANGE_IN_AREA &&
            dblChangeInWidth < MAX_CHANGE_IN_WIDTH &&
            dblChangeInHeight < MAX_CHANGE_IN_HEIGHT) {
            vectorOfMatchingChars.push_back(possibleMatchingChar);      // if the chars are a match, add the current char to vector of matching chars
        }
    }

    return(vectorOfMatchingChars);          // return result
}

